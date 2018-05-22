'use strict'

const app = require('express')(),
    request = require('request'),
    bodyParser = require('body-parser'),
    gridservice = require('./build/Release/matching_service_addon');


const gcs = new gridservice.Matching_Service();

app.use(bodyParser.json())
app.use(bodyParser.urlencoded({
    extended: false
}))

app.post('/updateloc', (req, res) => {
    gcs.update(parseFloat(req.body.lat), parseFloat(req.body.lon), req.body.uid, parseInt(req.body.timestamp));
    console.log("updated!");
    res.sendStatus(200)
})

app.post('/requesthelp', (req, res) => {
    var candidates = gcs.search(parseFloat(req.body.lat), parseFloat(req.body.lon));
    console.log(candidates);
    res.sendStatus(200)
})

app.listen(8000);