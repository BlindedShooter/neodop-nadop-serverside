'use strict'

const app = require('express')(),
    request = require('request'),
    bodyParser = require('body-parser'),
    gridservice = require('./build/Release/matching_service_addon');

var async = require('async');

const gcs = new gridservice.Matching_Service();
const minimum_helpers = 1;
const minimum_rating = 1;
var ongoing_help = new Map();

var admin = require("firebase-admin");
var serviceAccount = require("./neodop-nadop-firebase-adminsdk-utczv-d5487d8c7f.json");
var dbclient = require('mongodb').MongoClient;

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
                        if (candidates[i]) {
                            userRef.doc(candidates[i]).get()
                                .then(doc => {
                                    if (!doc.exists) {
                                        console.log('[Requesthelp] ==== No such helper! ====');
                                        if (i == len - 1) {
                                            console.log("[Requesthelp] =======> Messaging Finished\n")
                                        }
                                    } else if (doc.data().rating && doc.data().rating < minimum_rating) {
                                        console.log("[Requesthelp] ==== Rating is too low: ", doc.data().rating, " uid: ", doc.id, "====");
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
                                                console.log('[Requesthelp] ==== Error sending message: ', error.errorInfo.message, " while to uid: ", doc.id, "====");
                                                if (i == len - 1) {
                                                    console.log("[Requesthelp] =======> Messaging Finished\n")
                                                }
                                            });
                                    }
                                })
                        }
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
        res.sendStatus(400);
    }
    else {
        userRef.doc(req.body.helpeeuid).get()
            .then(doc => {
                if (!doc.exists) {
                    console.log('[Accepthelp] No such helper! (document not exists in firestore)\n');
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
                            console.log("[Accepthelp] ======== matching success ========\n");
                        })
                        .catch((error) => {
                            console.log("[Accepthelp] ======== Error Sending Message: ", error.errorInfo.message);
                        })
                }
            })
        ongoing_help.set(help_code, 1);
        res.sendStatus(200);
    }
})

app.post('/finishhelp', (req, res) => {
    var help_code = req.body.helpeeuid;
    var rating = req.body.rating;
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

app.post('/cancel_help', (req, res) => {
    var help_code = req.body.helpeeuid;
    console.log('[CancleHelp] helper: ', req.body.helperuid, " helpee: ", req.body.helpeeuid)
    var req_type = 'canceledby_helper';
    if (ongoing_help.has(help_code)) {
        ongoing_help.delete(help_code);
        userRef.doc(req.body.helpeeuid).get()
            .then(doc => {
                if (!doc.exists) {
                    console.log('[CancleHelp] No such helpee! (document not exists in firestore)\n');
                } else {
                    var message = {
                        data: {
                            type: req_type
                        },
                        token: doc.data().token
                    };
                    admin.messaging().send(message)
                        .then((response) => {
                            console.log("[CancleHelp] ======== matching canceled ========\n");
                        })
                        .catch((error) => {
                            console.log("[CancleHelp] ======== Error Sending Message: ", error.errorInfo.message);
                        })
                }
            })
        res.set('Content-Type', 'text/plain');
        res.status(200);
        res.send('Successfully Canceled');
    }
    else {
        res.set('Content-Type', 'text/plain');
        res.status(400);
        res.send('No ongoing help!');
    }
})

app.post('/cancel_request', (req, res) => {
    var help_code = req.body.helpeeuid;
    console.log('[CancleReq] helper: ', req.body.helperuid, " helpee: ", req.body.helpeeuid)
    var req_type = 'canceledby_disabled';
    if (ongoing_help.has(help_code)) {
        ongoing_help.delete(help_code);
        userRef.doc(req.body.helperuid).get()
            .then(doc => {
                if (!doc.exists) {
                    console.log('[CancleReq] No such helper! (document not exists in firestore)');
                } else {
                    var message = {
                        data: {
                            type: req_type
                        },
                        token: doc.data().token
                    };
                    admin.messaging().send(message)
                        .then((response) => {
                            console.log("[CancleReq] ======== matching cancelled ========\n");
                        })
                        .catch((error) => {
                            console.log("[CancleReq] ======== Error Sending Message: ", error.errorInfo.message);
                        })
                }
            })
        res.set('Content-Type', 'text/plain');
        res.status(200);
        res.send('Successfully Canceled');
    }
    else {
        res.set('Content-Type', 'text/plain');
        res.status(400);
        res.send('No ongoing help!');
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
