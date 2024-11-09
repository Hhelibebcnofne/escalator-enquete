import { ChangeEvent, FC, useState } from "react";
import { Question } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";

interface UpdateQuestionsProps {
  question: Question;
}

const Update: FC<UpdateQuestionsProps> = ({ question }) => {
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
    fetch(`/questions/${question.id}`, {
      method: "put",
      headers: csrfToken
        ? {
            "Content-Type": "application/json",
            "X-CSRF-Token": csrfToken,
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
      <h1>Update Question</h1>
      <form onSubmit={handleFormSubmit}>
        <input
          id="questionText"
          name="questionText"
          type="text"
          value={sentence}
          onChange={handleSentenceChange}
          required={true}
        />
        <input
          id="optionAText"
          name="optionAText"
          type="text"
          value={optionAText}
          onChange={handleOptionATextChange}
          required={true}
        />
        <input
          id="optionBText"
          name="optionBText"
          type="text"
          value={optionBText}
          onChange={handleOptionBTextChange}
          required={true}
        />
        <button type="submit">Create Question</button>
      </form>
    </div>
  );
};

export default Update;
