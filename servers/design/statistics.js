// グラフのインスタンスを保存するための配列を作成
let chartArray = [];

//数値を変数に代入
let question_1 = "質問文1";
var question_2 = "質問文2";
var question_3 = "質問文3";
var previous = question_1;
var kao = "myChart1";
let myChart = "myChart"


// 数値を変数に代入
let Option_A = 20;
let Option_B = 10;
let Total = Option_A + Option_B;
const A = "🐶 犬";
const B = "🐱 猫";


setInterval(() => {
  //ここで質問文の更新
  question_1 = "わっはーーーーーーーーーーーーーーー";
  //もし、質問文が追加されていた場合。
  for (let i = 0; previous != question_1; i++) {
    // 値の更新
    previous = question_1;
  }
}, 3000);

// グラフへ値を入れる処理
// データをオブジェクト化。配列で複数のグラフを生成。
let myObjects   = [
  {ka: kao,  // キャンバスID
    key: question_1,
    Choice_Name_B: B,
    Choice_Name_A: A,
    value_A: Option_A,
    value_B: Option_B,
    value_C: Total
  //削除しても大丈夫
  },
  {
    ka: "myChart2",
    key: "質問2",
    Choice_Name_B: "選択肢B2",
    Choice_Name_A: "選択肢A2",
    value_A: 40,
    value_B: 60,
    value_C: 80
  }
];

myObjects.forEach((myObject, index) => {
// Chart.jsを使ってグラフを描画１
const ctx = document.getElementById(myObject.ka).getContext('2d');
// 配列にChartインスタンスを追加
chartArray[index] = new Chart(ctx, {
  type: 'bar',  // グラフの種類: 'bar', 'line', 'pie', など
  data: {
    labels: [myObject.key],  // ラベル
    datasets: [{
      //↓ここの質問も適応できるようにできればいいな。
      label: myObject.Choice_Name_B,//ここに値を入れたい。
      data: [myObject.value_A, myObject.value_C],  // データ
      backgroundColor: 'rgba(255, 99, 132, 0.2)',
      borderColor: 'rgba(255, 99, 132, 1)',
      borderWidth: 1
    }, {
      label: myObject.Choice_Name_A,
      //上限100を設定。できれば合計人数にしたい所存。
      //現在のイメージ：100人中の20人みたいな。
      data: [myObject.value_B, myObject.value_C],  // データ
      backgroundColor: 'rgba(54, 162, 235, 0.2)',
      borderColor: 'rgba(54, 162, 235, 1)',
      borderWidth: 1
    }]
  },
  options: {
    indexAxis: 'y',//これで横棒グラフに変更
    scales: {
      y: {
        beginAtZero: true
      }
    }
  }
});
});




// 初期値をHTMLに挿入
document.getElementById("number_Option_A").textContent = Option_A;
document.getElementById("number_Option_B").textContent = Option_B;
document.getElementById("number_Total").textContent = Total;
document.getElementById("number_A").textContent = A;
document.getElementById("number_B").textContent = B;
document.getElementById("myChart").textContent = ka;