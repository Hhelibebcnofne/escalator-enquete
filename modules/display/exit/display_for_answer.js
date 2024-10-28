let right_answer_description = "🐶 犬";
let left_answer_description = "🐱 猫";
//let aa = "左,右";
let aa = "パン,コメ";
//let aa = "left_count,right_count";
let a = aa.split(",");
let right_return = a[1];
let left_return = a[0];
let right_counter = 0;
let left_counter = 0;
let total_person_count = 0;

const serviceUUID = 0xFFE0;
const serialUUID = 0xFFE1;
let device;
let serialCharacteristic;

if (right_return == "right_count" && left_return == "left_count") {
} else if (right_return != right_answer_description && left_return != left_answer_description) {
  right_answer_description = right_return;
  left_answer_description = left_return;
}

function count_up(reception_text) {
  console.log(reception_text);

  if (reception_text == "right_count") {
    right_counter += 1;

  } else if (reception_text == "left_count") {
    left_counter += 1;

  } else if (reception_text == right_return) {
    right_counter += 1;

  } else if (reception_text == left_return) {
    left_counter += 1;

  } else {
    console.log("error");
    return;
  }

  animation_for_bar(reception_text);
  total_person_count = right_counter + left_counter;
  let right_answer_ratio = Math.floor((right_counter / total_person_count * 100));
  let left_answer_ratio = 100 - right_answer_ratio;
  right_answer_ratio = right_answer_ratio.toString()
  left_answer_ratio = left_answer_ratio.toString()

  document.getElementById("right-answer-ratio").textContent = right_answer_ratio + "%";
  document.querySelector(".right-answer-ratio-bar-inner").style.width = right_answer_ratio + "%";//グラフの幅を変更
  // HTML内の数値とグラフの幅を更新
  document.getElementById("left-answer-ratio").textContent = left_answer_ratio + "%";
  document.querySelector(".left-answer-ratio-bar-inner").style.width = left_answer_ratio + "%";//グラフの幅を変更

  document.getElementById("total-person-count").textContent = total_person_count;
}


const sleep = (time) => new Promise((r) => setTimeout(r, time));//timeはミリ秒
async function animation_for_bar(reception_text) {
  let element_name;
  if (reception_text === right_return) {
    element_name = "right";
  } else {
    element_name = "left"
  }

  document.querySelector(`.${element_name}-answer-bar`).classList.add("zoom-in");
  await sleep(2000);
  document.querySelector(`.${element_name}-answer-bar`).classList.remove("zoom-in");
}


window.onload = function () {
  // 初期値をHTMLに挿入
  document.getElementById("right-answer-ratio").textContent = right_counter + "%";
  document.getElementById("left-answer-ratio").textContent = left_counter + "%";
  document.getElementById("total-person-count").textContent = 0;
  document.getElementById("right-answer-description").textContent = right_answer_description;
  document.getElementById("left-answer-description").textContent = left_answer_description;

  // 初期グラフ幅の設定
  document.querySelector(".right-answer-ratio-bar-inner").style.width = "0" + "%";
  document.querySelector(".left-answer-ratio-bar-inner").style.width = "0" + "%";

  bluetooth_connect_button_element.addEventListener("click", connect);
}


bluetooth_connect_button_element = document.getElementById('bluetooth-connect-button');
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
  bluetooth_connect_button_element.removeEventListener("click", connect);
  bluetooth_connect_button_element.addEventListener("click", disconnect);
  bluetooth_connect_button_element.textContent = "Disconnect";
}


function disconnect() {
  device.gatt.disconnect();
  bluetooth_connect_button_element.removeEventListener("click", disconnect);
  bluetooth_connect_button_element.addEventListener("click", connect);
  bluetooth_connect_button_element.textContent = "Connect";
}


function read(event) {
  let buffer = event.target.value.buffer;
  let view = new Uint8Array(buffer);
  let decoder = new TextDecoder("utf-8");
  let decodedMessage = decoder.decode(view).trim();
  count_up(decodedMessage);
}

// サンプル動作用
let c = 0
setInterval(() => {
  if (c % 2 == 0) {
    count_up(left_return);//左
  } else {
    count_up(right_return);//右
  }
  c += 1
}, 3000);