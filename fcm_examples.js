'use strict'

const app = require('express')(),
    request = require('request'),
    mongo = require('mongodb'),
    bodyParser = require('body-parser')
    gridservice = require('./build/Release/matching_service_addon')

gcs = new gridservice.Matching_Service();

app.use(bodyParser.json())
app.use(bodyParser.urlencoded({
    extended: false
}))

const MongoClient = mongo.MongoClient

const url = 'mongodb://localhost:27017/neodop-nadop-fcm'

app.get('/notifications', (req, res) => {
    res.sendFile(__dirname + '/notifcenter.html')
})

app.post('/store', (req, res) => {
    MongoClient.connect(url, (err, client) => {
            var db1 = client.db('neodop-nadop-fcm');
            console.log(req.body);
            db1.collection('tokens').insertOne(req.body, (err, body) => {
                if (err) throw err
                res.sendStatus(200)
            })
            client.close()
    })
})

app.post('/updateloc', (req, res) => {
    gcs.update(req.body.lat, req.body.lon, req.body.uid, req.body.timestamp);
    res.sendStatus(200)
})

app.post('/requesthelp', (req, res) => {
    var candidates = gcs.search(req.body.lat, req.body.lon);
    console.log(candidates);
})

const sendNotifications = (data) => {

    const dataString = JSON.stringify(data)

    const headers = {
        'Authorization': 'key=AIzaSyAga47GxuAWeH4-Wb42Le4KgACK1jleO3M',
        'Content-Type': 'application/json',
        'Content-Length': dataString.length
    }

    const options = {
        uri: 'https://fcm.googleapis.com/fcm/send',
        method: 'POST',
        headers: headers,
        json: data
    }

    request(options, function (err, res, body) {
        if (err) throw err
        else console.log(body)
    })
}

const sendToTopics = (msg, title, topic, response) => {

    const data = {
        "data": {
            "body": msg,
            "title": title
        }
    }

    data['to'] = '/topics/' + topic

    sendNotifications(data)

    response.sendStatus(200)
}


const sendToAll = (msg, title, regIdArray, response) => {

    const data = {
        "data": {
            "body": msg,
            "title": title
        }
    }

    const folds = regIdArray.length % 1000

    for (let i = 0; i < folds; i++) {
        let start = i * 1000,
            end = (i + 1) * 1000

        data['registration_ids'] = regIdArray.slice(start, end).map((item) => {
            return item['token']
        })

        sendNotifications(data)
    }

    response.sendStatus(200)
}

app.post('/notify', (req, res) => {

    let msg = req.body.message,
        title = req.body.title,
        type = req.body.type,
        topic = req.body.topic

    if (type === 'topic') {
        sendToTopics(msg, title, topic, res)
    } else {
        MongoClient.connect(url, function (err, client) {
                var db = client.db('neodop-nadop-fcm');
                db.collection('tokens').find({}).toArray((err, docs) => {
                    sendToAll(msg, title, docs, res)
                })
                client.close();
        })
    }
})

app.listen(8000);