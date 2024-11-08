import { ChangeEvent, FC, useState } from "react";
import { getCsrfToken } from "../../utils/csrf";

const CreateQuestion: FC = () => {
  const [sentence, setSentence] = useState("");
  const handleSentenceChange = (event: ChangeEvent<HTMLInputElement>) => {
    setSentence(event.target.value);
  };
  const [optionAText, setOptionAText] = useState("");
  const handleOptionATextChange = (event: ChangeEvent<HTMLInputElement>) => {
    setOptionAText(event.target.value);
  };
  const [optionBText, setOptionBText] = useState("");
  const handleOptionBTextChange = (event: ChangeEvent<HTMLInputElement>) => {
    setOptionBText(event.target.value);
  };

  const csrfToken = getCsrfToken();

  const handleFormSubmit = (event: ChangeEvent<HTMLFormElement>) => {
    event.preventDefault();
    fetch(`/questions`, {
      method: "post",
      headers: csrfToken
        ? {
            "Content-Type": "application/json",
            "X-CSRFToken": csrfToken,
          }
        : { "Content-Type": "application/json" },
      body: JSON.stringify({
        sentence: sentence,
        optionA: optionAText,
        optionB: optionBText,
      }),
    }).then(() => {
      window.location.href = "/questions";
    });
  };

  return (
    <div>
      <h1>Create Question</h1>
      <form onSubmit={handleFormSubmit}>
        <input
          id="questionText"
          name="questionText"
          type="text"
          value={sentence}
          onChange={handleSentenceChange}
        />
        <input
          id="optionAText"
          name="optionAText"
          type="text"
          value={optionAText}
          onChange={handleOptionATextChange}
        />
        <input
          id="optionBText"
          name="optionBText"
          type="text"
          value={optionBText}
          onChange={handleOptionBTextChange}
        />
        <button type="submit">Create Question</button>
      </form>
    </div>
  );
};

export default CreateQuestion;
