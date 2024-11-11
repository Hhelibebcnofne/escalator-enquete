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
  const handlePublishQuestion = (id: number) => {
    fetch(`/questions/publish/${id}`, {
      method: "post",
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
      <table>
        <thead>
          <tr>
            <th>Id</th>
            <th>Sentence</th>
            <th>Option A</th>
            <th>Option B</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          {questions.map((question) => (
            <tr key={question.id}>
              <td>{question.id}</td>
              <td>{question.sentence}</td>
              <td>{question.optionA}</td>
              <td>{question.optionB}</td>
              <td>
                <div>
                  <a
                    onClick={() => handlePublishQuestion(question.id)}
                    style={{ cursor: "pointer" }}
                  >
                    Publish{" "}
                  </a>

                  <a href={`/questions/edit/${question.id}`}>Update </a>
                  <a
                    onClick={() => handleDeleteQuestion(question.id)}
                    style={{ cursor: "pointer" }}
                  >
                    Delete
                  </a>
                </div>
              </td>
            </tr>
          ))}
        </tbody>
      </table>
      <a href="/questions/create">Create Question</a>
    </div>
  );
};

export default ReadQuestions;