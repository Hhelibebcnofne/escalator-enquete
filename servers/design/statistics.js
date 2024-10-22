// グラフのインスタンスを保存するための配列を作成
let chartArray = [];

let myObjects = [{
  canvasId: "myChart1",
  question: "質問1",
  Choice_Name_B: "選択肢B1",
  Choice_Name_A: "選択肢A1",
  value_A: 100,
  value_B: 160,
  value_C: 260

},
{
  canvasId: "myChart2",
  question: "質問2",
  Choice_Name_B: "選択肢B2",
  Choice_Name_A: "選択肢A2",
  value_A: 40,
  value_B: 60,
  value_C: 100

},
]

// Canvasを追加する関数
function createCanvas(canvasId) {
  const canvas = document.createElement('canvas');
  canvas.id = canvasId;
  canvas.width = 400;
  canvas.height = 200;
  document.getElementById('canvas_list').appendChild(canvas);
  return canvas;
}

// グラフを生成する関数
function generateGraph(myObject) {
  // 新しいCanvasを生成して追加
  const canvas = createCanvas(myObject.canvasId);

  // グラフのコンテキストを取得
  const ctx = canvas.getContext('2d');

  // 新しいグラフを作成
  const newChart = new Chart(ctx, {
    type: 'bar',
    data: {
      labels: [myObject.question],
      datasets: [{
        label: myObject.Choice_Name_B,
        data: [myObject.value_A, myObject.value_C],
        backgroundColor: 'rgba(255, 99, 132, 0.2)',
        borderColor: 'rgba(255, 99, 132, 1)',
        borderWidth: 1
      }, {
        label: myObject.Choice_Name_A,
        data: [myObject.value_B, myObject.value_C],
        backgroundColor: 'rgba(54, 162, 235, 0.2)',
        borderColor: 'rgba(54, 162, 235, 1)',
        borderWidth: 1
      }]
    },
    options: {
      indexAxis: 'y',
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });

  // 新しいグラフを配列に追加
  chartArray.push(newChart);
}

// 初期グラフ生成
function init() {
  // 初期状態でmyObjectsに基づいてグラフを生成
  myObjects.forEach((myObject) => {
    generateGraph(myObject);  // グラフを生成
  });
}

// ページ読み込み時に一度だけ初期化を実行
window.onload = function () {
  init();  // 最初の1回だけグラフを生成
};
