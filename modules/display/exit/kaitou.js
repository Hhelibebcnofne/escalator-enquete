// 数値70を変数に代入
var inu = 70;
var neko = 30;
var ninzuu = inu+neko;
var previousInu = inu; //前回のinuの値を保存
var previousInu2 = neko; //前回のnekoの値を保存
const A = "🐶 犬";
const B = "🐱 猫";



// 3秒ごとにinuのカウントを1ずつ増やす
setInterval(() => {
  // inuの値を+1
  inu += 1;

  // 前回の値と比較し、カウントが増えた場合のみアニメーションを実行
  if (inu !== previousInu) {
    ninzuu = inu+neko;
    // HTML内の数値とグラフの幅を更新
    document.getElementById("number_inu").textContent = parseFloat((inu/ninzuu*100).toFixed(0));
    document.querySelector(".dog-bar-inner").style.width = parseFloat((inu/ninzuu*100).toFixed(0)) + "%";//グラフの幅を変更
    // HTML内の数値とグラフの幅を更新
    document.getElementById("number_neko").textContent = parseFloat((neko/ninzuu*100).toFixed(0));
    document.querySelector(".cat-bar-inner").style.width = parseFloat((neko/ninzuu*100).toFixed(0)) + "%";//グラフの幅を変更
    
    // zoom-inアニメーションを適用
    const barInner = document.querySelector(".result-bar");
    barInner.classList.add("zoom-in");//result-barにzoom-inを追加。
    
    //アニメーションが終わったら削除してリセット
    setTimeout(() => {
      barInner.classList.remove("zoom-in");
    }, 2000);  // 2秒後にリセット
    
    // 前回のinuの値を更新
    previousInu = inu;
  }
  
  ninzuu = inu+neko;
  document.getElementById("number_hito").textContent = ninzuu;
  /*ここで合計人数の値を更新している。*/
}, 3000);




// 5秒ごとにnekoのカウントを1ずつ増やす
setInterval(() => {
  // inuの値を+1
  neko += 5;

  // 前回の値と比較し、カウントが増えた場合のみアニメーションを実行
  if (neko !== previousInu2) {
    ninzuu = inu+neko;
    // HTML内の数値とグラフの幅を更新
    document.getElementById("number_inu").textContent = parseFloat((inu/ninzuu*100).toFixed(0));
    document.querySelector(".dog-bar-inner").style.width = parseFloat((inu/ninzuu*100).toFixed(0)) + "%";//グラフの幅を変更
    // HTML内の数値とグラフの幅を更新
    document.getElementById("number_neko").textContent = parseFloat((neko/ninzuu*100).toFixed(0));
    document.querySelector(".cat-bar-inner").style.width = parseFloat((neko/ninzuu*100).toFixed(0)) + "%";//グラフの幅を変更
    
    // zoom-inアニメーションを適用
    const barInner = document.querySelector(".result-bar2");
    barInner.classList.add("zoom-in");//result-barにzoom-inを追加。
    
    //アニメーションが終わったら削除してリセット
    setTimeout(() => {
      barInner.classList.remove("zoom-in");
    }, 2000);  // 2秒後にリセット
    
    // 前回のinuの値を更新
    previousInu2 = neko;
  }
  
  ninzuu = inu+neko;
  document.getElementById("number_hito").textContent = ninzuu;
  /*ここで合計人数の値を更新している。*/
}, 5000);




// 初期値をHTMLに挿入
document.getElementById("number_inu").textContent = inu;
document.getElementById("number_neko").textContent = neko;
document.getElementById("number_hito").textContent = ninzuu;
document.getElementById("number_A").textContent = A;
document.getElementById("number_B").textContent = B;

// 初期グラフ幅の設定
document.querySelector(".dog-bar-inner").style.width = inu + "%";
document.querySelector(".cat-bar-inner").style.width = neko + "%";