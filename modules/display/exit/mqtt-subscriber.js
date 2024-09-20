let messageCount1 = 0;
let messageCount2 = 0;

const clientId = 'mqtt_subscriber' + Math.random().toString(16).substr(2, 8);
const host = 'ws://broker.emqx.io:8083/mqtt';

const options = {
    keepalive: 60,
    clientId: clientId,
    protocolId: 'MQTT',
    protocolVersion: 4,
    clean: true, // 要検討：再接続時どうするか
    reconnectPeriod: 1000,
    connectTimeout: 30 * 1000,
};

console.log('Connecting mqtt client for Subscriber');

const client = mqtt.connect(host, options);

client.on("connect", () => {
    console.log("Subscriber connected to broker");
    // 動作確認用だからemqxに同じような条件で同じトピックあったら非常に問題
    client.subscribe('presence1', (err) => {
        if (!err) {
            console.log("Subscribed to presence1 topic");
        }
    });
    client.subscribe('presence2', (err) => {
        if (!err) {
            console.log("Subscribed to presence2 topic");
        }
    });
});

client.on("message", (topic, message) => {
    console.log(`Received message from topic: ${topic}`);
    // 要検討：トピックで分けるかメッセージifで分けるか
    if (topic === 'presence1') {
        messageCount1++;
        document.getElementById('messageCount1').innerHTML = "右カウント数: " + messageCount1;
    } else if (topic === 'presence2') {
        messageCount2++;
        document.getElementById('messageCount2').innerHTML = "左カウント数: " + messageCount2;
    }
});
