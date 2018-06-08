# neodop-nadop-serverside
This is a server implementation for 'Neodop Nadop' service. Used node.js for server implementation, and node.js C++ addon for matching algorithm implementation. Used Google Firebase Firestore as a database, and Firebase Cloud Messaging to signal the users.

Installation: npm install

It can require node-gyp to build a C++ addon for it, so use command below if npm install not works.

npm install express request body-parser firebase-admin --save

Also, it requires Firebase Admin SDK's credential file for the Admin SDK. (The file named <Project Name>-firebase-admin-sdk-utczv-<some code>.json) 

혹시 필요하시다면 3조 이지훈에게 연락주시면 됩니다 조교님.
