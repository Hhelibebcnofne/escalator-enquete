const serviceUUID = 0xFFE0;
const serialUUID = 0xFFE1;

let device;
let serialCharacteristic;

async function connect() {
    device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [serviceUUID] }]
    });

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(serviceUUID);

    serialCharacteristic = await service.getCharacteristic(serialUUID);

    // 通知の開始
    await serialCharacteristic.startNotifications();
    serialCharacteristic.addEventListener('characteristicvaluechanged', read);

    // UI更新
    document.getElementById('connect').removeEventListener("click", connect);
    document.getElementById('connect').addEventListener("click", disconnect);
    document.getElementById('connect').textContent = "Disconnect";
}

function disconnect() {
    device.gatt.disconnect();
    document.getElementById('connect').removeEventListener("click", disconnect);
    document.getElementById('connect').addEventListener("click", connect);
    document.getElementById('connect').textContent = "Connect";
}

// データを受信した際に呼ばれる関数
function read(event) {
    let buffer = event.target.value.buffer;
    let view = new Uint8Array(buffer);
    let decodedMessage = String.fromCharCode.apply(null, view);

    let newNode = document.createElement('p');
    newNode.classList.add("received-message");
    newNode.textContent = decodedMessage;

    document.getElementById("terminal").appendChild(newNode);

    let placeholder = document.getElementsByClassName('placeholder');
    if (placeholder.length != 0) placeholder[0].remove();
}

// ブラウザからメッセージを送信する関数
async function write() {
    let message = document.getElementById("message-input").value;
    message += '\n'; // 改行を追加して終端を示す
    let buffer = new ArrayBuffer(message.length);
    let encodedMessage = new Uint8Array(buffer);
    
    for (let i = 0; i < message.length; i++) {
        encodedMessage[i] = message.charCodeAt(i);
    }

    // 書き込み処理
    await serialCharacteristic.writeValue(encodedMessage);
}

// 接続ボタンと送信ボタンにイベントリスナーを設定
document.getElementById('connect').addEventListener("click", connect);
document.getElementById('send').addEventListener("click", write);
