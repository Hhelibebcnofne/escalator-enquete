const express = require('express');
const mqtt = require('mqtt');
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
let messageCount1 = 0;
let messageCount2 = 0;

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: MQTT_USERNAME,
  password: MQTT_PASSWORD,
  reconnectPeriod: 1000,
});

// const topic = '/nodejs/mqtt';

client.on('connect', () => {
  console.log('Connected to MQTT Broker');

  client.subscribe([MQTT_TOPIC], () => {
    console.log(`Subscribed to topic '${MQTT_TOPIC}'`);
  });
});

client.on('message', (MQTT_TOPIC, payload) => {
  let payload_txt = payload.toString();
  if (payload_txt === '0') {
    messageCount1++;
    console.log("右カウント数: ", messageCount1);
  } else if (payload_txt === '1') {
    messageCount2++;
    console.log("左カウント数: ", messageCount2);
  }
});

// 静的ファイル（HTML）を提供
app.use(express.static(path.join(__dirname, 'public')));

// カウント数を提供するエンドポイント
app.get('/counts', (req, res) => {
  res.json({ messageCount1, messageCount2 });
});

// サーバーを起動
app.listen(MQTT_WEBPORT, () => {
  console.log(`Server is running on http://localhost:${MQTT_WEBPORT}`);
});
