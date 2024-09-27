const express = require('express');
const mqtt = require('mqtt');
const path = require('path');

const app = express();
const host = 'broker.emqx.io';
const port = '1883'; 
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;  

const connectUrl = `mqtt://${host}:${port}`;   
let messageCount1 = 0;
let messageCount2 = 0;

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: 'emqx',
  password: 'subscrib',
  reconnectPeriod: 1000,
});

const topic = '/nodejs/mqtt';

client.on('connect', () => {
  console.log('Connected to MQTT Broker');

  client.subscribe([topic], () => {
    console.log(`Subscribed to topic '${topic}'`);
  });
});

client.on('message', (topic, payload) => {
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
const webPort = 3000;
app.listen(webPort, () => {
  console.log(`Server is running on http://localhost:${webPort}`);
});
