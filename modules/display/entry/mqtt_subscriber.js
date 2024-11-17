const express = require('express');
const mqtt = require('mqtt');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
require('dotenv').config()

const app = express();
const MQTT_HOST = process.env.MQTT_HOST;
const MQTT_PORT = process.env.MQTT_PORT;
const MQTT_TOPIC = process.env.MQTT_TOPIC;
const MQTT_USERNAME = process.env.MQTT_USERNAME;
const MQTT_PASSWORD = process.env.MQTT_PASSWORD;
const MQTT_WEBPORT = Number(process.env.MQTT_WEBPORT);

const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectUrl = `mqtt://${MQTT_HOST}:${MQTT_PORT}`;

const server = http.createServer(app);

const mqtt_client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: MQTT_USERNAME,
  password: MQTT_PASSWORD,
  reconnectPeriod: 1000,
});

const wss = new WebSocket.Server({ server });
wss.on('connection', (ws) => {
  console.log('WebSocket client connected');
});



mqtt_client.on('connect', () => {
  console.log('Connected to MQTT Broker');

  mqtt_client.subscribe([MQTT_TOPIC], () => {
    console.log(`Subscribed to topic '${MQTT_TOPIC}'`);
  });
});

mqtt_client.on('message', (topic, payload) => {
  console.log(`MQTT Message: ${payload.toString()}`);
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(payload.toString());
    }
  });
});

// app.use(express.static(path.join(__dirname, 'public')));


// サーバーを起動
server.listen(MQTT_WEBPORT, () => {
  console.log(`Server is running on http://localhost:${MQTT_WEBPORT}`);
});
