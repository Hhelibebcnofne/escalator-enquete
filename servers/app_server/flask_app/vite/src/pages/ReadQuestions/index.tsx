import { FC } from "react";
import { Questions } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";

interface ReadQuestionsProps {
  questions: Questions;
}

const ReadQuestions: FC<ReadQuestionsProps> = ({ questions }) => {
  const csrfToken = getCsrfToken();
  const handleDeleteQuestion = (id: number) => {
    fetch(`/questions/${id}`, {
      method: "delete",
      headers: csrfToken
        ? {
            "X-CSRF-Token": csrfToken,
          }
        : {},
    }).then(() => {
      window.location.href = "/questions";
    });
  };
  return (
    <div>
      <h1>Read Questions</h1>
      {questions.map((question) => (
        <>
          <div key={question.id}>
            <p>
              {question.id} {question.sentence} {question.optionA}{" "}
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
      ))}
      <a href="/questions/create">Create Question</a>
    </div>
  );
};

export default ReadQuestions;
