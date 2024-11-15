let right_answer_description = "🐶 犬";
let left_answer_description = "🐱 猫";
let right_counter = 0;
let left_counter = 0;
let total_person_count = 0;

const serviceUUID = 0xFFE0;
const serialUUID = 0xFFE1;
let device;
let serialCharacteristic;

function count_up(reception_text) {
  console.log(reception_text);
  if (right_counter == 0 && left_counter == 0) {
    const barElement = document.getElementById('myBar'); // IDで要素を取得
    barElement.style.backgroundColor = '#00b1a9'; // 背景色を#dddに設定
  }

  if (reception_text == "right_count") {
    right_counter += 1;

  } else if (reception_text == "left_count") {
    left_counter += 1;

  } else if (reception_text == "count_error") {
    console.error("sensor count error");
    return;
  } else if ((reception_text.split(",").length) == 2) {
    const questions = reception_text.split(",")
    document.getElementById("right-answer-description").textContent = questions[1];
    document.getElementById("right-answer-ratio").textContent = questions[0];

    right_counter = 0;
    left_counter = 0;
    total_person_count = 0;

    // 表示の更新
    document.getElementById("left-answer-description").textContent = "0%";
    document.getElementById("left-answer-ratio").textContent = "0%";
    document.querySelector(".right-answer-ratio-bar-inner").style.width = "0%";
    document.querySelector(".left-answer-ratio-bar-inner2").style.width = "100%";
    document.getElementById("total-person-count").textContent = total_person_count;
    if (right_counter == 0 && left_counter == 0) {
      const barElement = document.getElementById('myBar'); // IDで要素を取得
      barElement.style.backgroundColor = '#ddd'; // 背景色を#dddに設定
    }
    return;
  } else {
    console.error("data format error");
  }
  // パーセンテージとグラフを再計算して表示更新
  animation_for_bar(reception_text);
  total_person_count = right_counter + left_counter;
  let right_answer_ratio = total_person_count > 0 ? Math.floor((right_counter / total_person_count) * 100) : 0;
  let left_answer_ratio = 100 - right_answer_ratio;
  right_answer_ratio = right_answer_ratio.toString();
  left_answer_ratio = left_answer_ratio.toString();

  document.getElementById("left-answer-description").textContent = right_answer_ratio + "%";
  document.querySelector(".right-answer-ratio-bar-inner").style.width = right_answer_ratio + "%";
  document.getElementById("left-answer-ratio").textContent = left_answer_ratio + "%";
  document.querySelector(".left-answer-ratio-bar-inner2").style.width = left_answer_ratio + "100%";
  document.getElementById("total-person-count").textContent = total_person_count;

}

const sleep = (time) => new Promise((r) => setTimeout(r, time));//timeはミリ秒
async function animation_for_bar(reception_text) {
  let element_name;
  let element_nam;
  if (reception_text === "right_count") {
    element_name = "right";
    element_nam = "left";
  } else {
    element_name = "right";
    element_nam = "left";
  }
  document.querySelector(`.${element_nam}-answer-bar`).classList.add("zoom-in");
  document.querySelector(`.${element_name}-answer-bar`).classList.add("zoom-in");
  await sleep(2000);
  document.querySelector(`.${element_nam}-answer-bar`).classList.remove("zoom-in");
  document.querySelector(`.${element_name}-answer-bar`).classList.remove("zoom-in");
}

window.onload = function () {
  // 初期値をHTMLに挿入
  document.getElementById("left-answer-description").textContent = right_counter + "%";
  document.getElementById("left-answer-ratio").textContent = left_counter + "%";
  document.getElementById("total-person-count").textContent = 0;
  document.getElementById("right-answer-description").textContent = right_answer_description;
  document.getElementById("right-answer-ratio").textContent = left_answer_description;


  // 初期グラフ幅の設定
  document.querySelector(".right-answer-ratio-bar-inner").style.width = "0" + "%";
  document.querySelector(".left-answer-ratio-bar-inner2").style.width = "100" + "%";

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
// let c = 0
// setInterval(() => {
//   if (c % 2 == 0) {
//     count_up("left_count");//左
//   } else {
//     count_up("right_count");//右
//   }

//   if ((c + 1) % 5 == 0) {
//     count_up("sample1,sample2");
//   }
//   c += 1
// }, 3000);