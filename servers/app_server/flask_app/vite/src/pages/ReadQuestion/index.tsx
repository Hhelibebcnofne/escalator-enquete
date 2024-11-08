import { FC } from "react";
import { Question } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";

interface ReadQuestionProps {
  question: Question;
}

const ReadQuestion: FC<ReadQuestionProps> = ({ question }) => {
  const csrfToken = getCsrfToken();
  const handleDeleteQuestion = (id: number) => {
    fetch(`/questions/${id}`, {
      method: "delete",
      headers: csrfToken ? { "X-CSRF-Token": csrfToken } : {},
    }).then(() => {
      window.location.href = "/questions";
    });
  };
  return (
    <>
      <h1>Read Question</h1>
      <div key={question.id}>
        <p>
          {question.id}
          {question.sentence}
          {question.optionA}
          {question.optionB}
        </p>
      </div>
      <div>
        <a href={`/questions/edit/${question.id}`}>Update </a>
        <a
          onClick={() => handleDeleteQuestion(question.id)}
          style={{ cursor: "pointer" }}
        >
          Delete
        </a>
      </div>
    </>
  );
};

export default ReadQuestion;
