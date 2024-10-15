// 数値70を変数に代入
// var inu = 70;
// var neko = 30;
let right_counter = 0;
let left_counter = 0;
let all_person_count = right_counter + left_counter;
// var previousInu = inu; //前回のinuの値を保存
// var previousInu2 = neko; //前回のnekoの値を保存
const A = "🐶 犬";
const B = "🐱 猫";


// // 3秒ごとにinuのカウントを1ずつ増やす
// setInterval(() => {
//   // inuの値を+1
//   inu += 1;

//   // 前回の値と比較し、カウントが増えた場合のみアニメーションを実行
//   if (inu !== previousInu) {
//     ninzuu = inu + neko;
//     // HTML内の数値とグラフの幅を更新
//     document.getElementById("number_inu").textContent = parseFloat((inu / ninzuu * 100).toFixed(0));
//     document.querySelector(".dog-bar-inner").style.width = parseFloat((inu / ninzuu * 100).toFixed(0)) + "%";//グラフの幅を変更
//     // HTML内の数値とグラフの幅を更新
//     document.getElementById("number_neko").textContent = parseFloat((neko / ninzuu * 100).toFixed(0));
//     document.querySelector(".cat-bar-inner").style.width = parseFloat((neko / ninzuu * 100).toFixed(0)) + "%";//グラフの幅を変更

//     // zoom-inアニメーションを適用
//     const barInner = document.querySelector(".result-bar");
//     barInner.classList.add("zoom-in");//result-barにzoom-inを追加。

//     //アニメーションが終わったら削除してリセット
//     setTimeout(() => {
//       barInner.classList.remove("zoom-in");
//     }, 2000);  // 2秒後にリセット

//     // 前回のinuの値を更新
//     previousInu = inu;
//   }

//   ninzuu = inu + neko;
//   document.getElementById("number_hito").textContent = ninzuu;
//   /*ここで合計人数の値を更新している。*/
// }, 3000);


// // 5秒ごとにnekoのカウントを1ずつ増やす
// setInterval(() => {
//   // inuの値を+1
//   neko += 5;

//   // 前回の値と比較し、カウントが増えた場合のみアニメーションを実行
//   if (neko !== previousInu2) {
//     ninzuu = inu + neko;
//     // HTML内の数値とグラフの幅を更新
//     document.getElementById("number_inu").textContent = parseFloat((inu / ninzuu * 100).toFixed(0));
//     document.querySelector(".dog-bar-inner").style.width = parseFloat((inu / ninzuu * 100).toFixed(0)) + "%";//グラフの幅を変更
//     // HTML内の数値とグラフの幅を更新
//     document.getElementById("number_neko").textContent = parseFloat((neko / ninzuu * 100).toFixed(0));
//     document.querySelector(".cat-bar-inner").style.width = parseFloat((neko / ninzuu * 100).toFixed(0)) + "%";//グラフの幅を変更

//     // zoom-inアニメーションを適用
//     const barInner = document.querySelector(".result-bar2");
//     barInner.classList.add("zoom-in");//result-barにzoom-inを追加。

//     //アニメーションが終わったら削除してリセット
//     setTimeout(() => {
//       barInner.classList.remove("zoom-in");
//     }, 2000);  // 2秒後にリセット

//     // 前回のinuの値を更新
//     previousInu2 = neko;
//   }

//   ninzuu = inu + neko;
//   document.getElementById("number_hito").textContent = ninzuu;
//   /*ここで合計人数の値を更新している。*/
// }, 5000);

function count_up(reception_text) {
  console.log(reception_text);
  if (reception_text == "right_count"){
    right_counter += 1;
  } else if (reception_text == "left_count"){
    left_counter += 1;
  } else {
    console.log("error");
    return;
  }

  console.log("test log");
  document.getElementById("number_inu").textContent = parseFloat((right_counter / all_person_count * 100).toFixed(0));
  document.querySelector(".dog-bar-inner").style.width = parseFloat((right_counter / all_person_count * 100).toFixed(0)) + "%";//グラフの幅を変更
  // HTML内の数値とグラフの幅を更新
  document.getElementById("number_neko").textContent = parseFloat((left_counter / all_person_count * 100).toFixed(0));
  document.querySelector(".cat-bar-inner").style.width = parseFloat((left_counter / all_person_count * 100).toFixed(0)) + "%";//グラフの幅を変更
  
  // zoom-inアニメーションを適用
  const barInner = document.querySelector(".result-bar");
  barInner.classList.add("zoom-in");//result-barにzoom-inを追加。

  all_person_count = right_counter + left_counter;
  

  document.getElementById("number_hito").textContent = all_person_count
}



window.onload = function(){
  // 初期値をHTMLに挿入
  document.getElementById("number_inu").textContent = right_counter;
  document.getElementById("number_neko").textContent = left_counter;
  document.getElementById("number_hito").textContent = all_person_count;
  document.getElementById("number_A").textContent = A;
  document.getElementById("number_B").textContent = B;
  
  // 初期グラフ幅の設定
  document.querySelector(".dog-bar-inner").style.width = "0" + "%";
  document.querySelector(".cat-bar-inner").style.width = "0" + "%";
}


// setInterval(() => {
//   count_up();
// }, 500);




















const serviceUUID = 0xFFE0;
const serialUUID = 0xFFE1;

let device;
let serialCharacteristic;
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

// 接続ボタンと送信ボタンにイベントリスナーを設定
bluetooth_connect_button_element.addEventListener("click", connect);