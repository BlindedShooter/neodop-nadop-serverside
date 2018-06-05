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
        console.log("lat: ", req.body.lat, ", lon: ", req.body.lon, " uid: ", req.body.uid, "  helper updated!\n");
        res.sendStatus(200);
    }
    else {
        console.log("argument error!");
        res.sendStatus(400);
    }
})

app.post('/requesthelp', (req, res) => {
    var candidates = gcs.search(parseFloat(req.body.lat), parseFloat(req.body.lon), 5.0, 5.0);
    var uid = req.body.uid;
    var req_type = 'request';
    if (Array.isArray(candidates)) {
        console.log("Help request from lat: ", req.body.lat, ", lon: ", req.body.lon, ", uid: ", uid);
        console.log("Candidates: ", candidates);
        console.log("\n=> Messaging Progress:");
        // What the hell of the callbacks...... Not understanble nor maintainable.
        if (candidates.length >= minimum_helpers) {
            userRef.doc(uid).get().then(doc => {
                if (!doc.exists) {
                    console.log('invalid helpee uid');
                    res.sendStatus(400);
                } else {
                    for (var i = 0, len = candidates.length; i < len; i++) {
                        userRef.doc(candidates[i]).get()
                            .then(doc => {
                                if (!doc.exists) {
                                    console.log('No such helper!');
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
                                            console.log('==== Successfully sent message:', response, " to uid: ", doc.id);
                                        })
                                        .catch((error) => {
                                            console.log('==== Error sending message:', error.errorInfo.message, " while to uid: ", doc.id);
                                        });
                                }
                            })
                    }
                    res.sendStatus(200);
                }
            })    
        }
    }
    else {
        console.log("Invalid help request from ", res.body.uid);

        res.sendStatus(400);
    }
})

app.post('/accepthelp', (req, res) => {
    var help_code = res.body.helperuid;
    var req_type = 'match_success';
    if (ongoing_help.has(help_code)) {
        res.sendStatus(400);
    }
    else {
        userRef.doc(req.body.helpeeuid).get()
            .then(doc=> {
                if (!doc.exists) {
                    console.log('No such helper!');
                } else {
                    var message = {
                        data: {
                            type: req_type,
                            helper_uid: res.body.helperuid
                        },
                        token: doc.data().token
                    };
                    admin.messaging().send(message)
                        .then((response) => {
                            console.log("======== matching success ========");
                        })
                        .catch((error) => {
                            console.log("======== Error Sending Message: ", error.errorInfo.message);
                        })
                }
            })
        ongoing_help.set(help_code, 1);
        res.sendStatus(200);
    }
})

app.post('/finishhelp', (req, res) => {
    var help_code = res.body.helperuid;
    if (ongoing_help.has(help_code)) {
        ongoing_help.delete(help_code);
        res.sendStatus(200);
    }
    else {
        res.sendStatus(400);
    }
})

app.listen(8000);