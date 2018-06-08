'use strict'

const app = require('express')(),
    request = require('request'),
    bodyParser = require('body-parser'),
    gridservice = require('./build/Release/matching_service_addon');

const gcs = new gridservice.Matching_Service();
const minimum_helpers = 1;
var ongoing_help = new Map();

var admin = require("firebase-admin");
var serviceAccount = require("./neodop-nadop-firebase-adminsdk-utczv-d5487d8c7f.json");

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
})

var firedb = admin.firestore();
var userRef = firedb.collection('users');


app.use(bodyParser.json())
app.use(bodyParser.urlencoded({
    extended: false
}))

app.post('/updateloc', (req, res) => {
    if (req.body.lat && req.body.lon && req.body.uid && req.body.timestamp) {
        var result = gcs.update(parseFloat(req.body.lat), parseFloat(req.body.lon), req.body.uid, parseInt(req.body.timestamp));
        //console.log("\n[Updateloc] lat: ", req.body.lat, ", lon: ", req.body.lon, " uid: ", req.body.uid, "  helper updated!\n");
        res.sendStatus(200);
    }
    else {
        //console.log("[Updateloc] argument error!\n");
        res.sendStatus(400);
    }
})

app.post('/requesthelp', (req, res) => {
    var candidates = gcs.search(parseFloat(req.body.lat), parseFloat(req.body.lon), 4.0, 10.0);
    var uid = req.body.uid;
    var req_type = 'request';
    var res_log = '';

    res.set('Content-Type', 'text/plain');

    if (Array.isArray(candidates)) {
        var len = candidates.length;
        res_log += "[Requesthelp] Help request from lat: " + req.body.lat + ", lon: " + req.body.lon + ", uid: " + uid + "\n[Requesthelp] => Candidates: " + candidates;
        console.log("[Requesthelp] Help request from lat: ", req.body.lat, ", lon: ", req.body.lon, ", uid: ", uid);
        console.log("[Requesthelp] => Candidates: ", candidates);
        // What the hell of the callbacks...... Not understanble nor maintainable.
        if (candidates.length >= minimum_helpers) {
            userRef.doc(uid).get().then(doc => {
                if (!doc.exists) {
                    res_log += '\n[Requesthelp] ==== Invalid helpee uid (document not exists on firestore) ====\n';
                    console.log('[Requesthelp] ==== Invalid helpee uid (document not exists on firestore) ====\n');
                    res.status(400);
                    res.send(res_log);
                } else {
                    res.status(200);
                    res.send(res_log);
                    console.log("[Requesthelp] =======> Messaging Progress:");
                    for (var i = 0; i < len; i++) {
                        userRef.doc(candidates[i]).get()
                            .then(doc => {
                                if (!doc.exists) {
                                    console.log('[Requesthelp] ==== No such helper! ====');
                                    if (i == len - 1) {
                                        console.log("[Requesthelp] =======> Messaging Finished\n")
                                    }
                                } else {
                                    var message = {
                                        data: {
                                            type: req_type,
                                            helpee_uid: uid,
                                            help_info: req.body.info
                                        },
                                        token: doc.data().token
                                    };
                                    admin.messaging().send(message)
                                        .then((response) => {
                                        // Response is a message ID string.
                                            console.log('[Requesthelp] ==== Successfully sent message:', response, " to uid: ", doc.id, "====");
                                            if (i == len - 1) {
                                                console.log("[Requesthelp] =======> Messaging Finished\n")
                                            }
                                        })
                                        .catch((error) => {
                                            console.log('[Requesthelp] ==== Error sending message:', error.errorInfo.message, " while to uid: ", doc.id, "====");
                                            if (i == len - 1) {
                                                console.log("[Requesthelp] =======> Messaging Finished\n")
                                            }
                                        });
                                }
                            })
                    }
                }
            })    
        }
        else {
            res_log += "\n[Requesthelp] Not enough helpers for " + uid + "\n";
            console.log("[Requesthelp] Not enough helpers for ", uid, "\n");
            res.status(400);
            res.send(res_log);
        }
    }
    else {
        res_log += "[Requesthelp] Invalid help request from " + uid + "\n";
        console.log("[Requesthelp] Invalid help request from ", uid, "\n");
        res.status(400);
        res.send(res_log);
    }
})

app.post('/accepthelp', (req, res) => {
    var help_code = req.body.helpeeuid;
    var helper_uid_ = req.body.helperuid;
    var req_type = 'match_success';
    if (ongoing_help.has(help_code)) {
        //res.sendStatus(400);
    }
    //else {
        userRef.doc(req.body.helpeeuid).get()
            .then(doc=> {
                if (!doc.exists) {
                    console.log('[Accepthelp] No such helper! (document not exists in firestore)');
                } else {
                    var message = {
                        data: {
                            type: req_type,
                            helper_uid: helper_uid_
                        },
                        token: doc.data().token
                    };
                    admin.messaging().send(message)
                        .then((response) => {
                            console.log("[Accepthelp] ======== matching success ========");
                        })
                        .catch((error) => {
                            console.log("[Accepthelp] ======== Error Sending Message: ", error.errorInfo.message);
                        })
                }
            })
        ongoing_help.set(help_code, 1);
        res.sendStatus(200);
    //}
})

app.post('/finishhelp', (req, res) => {
    var help_code = req.body.helpeeuid;
    if (ongoing_help.has(help_code)) {
        ongoing_help.delete(help_code);
        res.sendStatus(200);
        console.log("[Finishhelp] Helping Finished, helper UID: ", help_code);
    }
    else {
        res.sendStatus(400);
        console.log("[Finishhelp] Invalid Finish Request, helper UID: ", help_code);
    }
})

app.get('/show_all_helpers', (req, res) => {
    res.set('Content-Type', 'text/plain');
    res.status(200);
    res.send("==== Available Helpers ====\n" + gcs.get_users());
})

app.get('/timestamp', (req, res) => {
    var t = new Date().getTime();
    res.set('Content-Type', 'text/plain');
    res.status(200);
    res.send('' + t);
})

console.log("======== Server Started ========\n");
app.listen(8000);
