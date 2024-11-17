const ws = new WebSocket('ws://localhost:3010');

ws.onopen = () => {
    console.log('WebSocket connected');
};

ws.onmessage = (event) => {
    console.log(event.data);

    message_object = JSON.parse(event.data);
    console.log(message_object);
    console.log(message_object.sentence);
    console.log(message_object.optionA);
    console.log(message_object.optionB);

    question_sentence = document.getElementById('question-sentence');
    optionA = document.getElementById('optionA');
    optionB = document.getElementById('optionB');


    question_sentence.innerHTML = message_object.sentence;
    optionA.innerHTML = message_object.optionA;
    optionB.innerHTML = message_object.optionB;
};