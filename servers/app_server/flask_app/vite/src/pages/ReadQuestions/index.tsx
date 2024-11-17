import { FC } from "react";
import { Questions } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";
import { Button, Table } from "react-bootstrap";

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
      <Table striped bordered hover>
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
                  <Button
                    className="m-1"
                    onClick={() => handlePublishQuestion(question.id)}
                  >
                    Publish
                  </Button>

                  <Button
                    className="m-1"
                    href={`/questions/edit/${question.id}`}
                  >
                    Update
                  </Button>
                  <Button
                    className="m-1"
                    onClick={() => handleDeleteQuestion(question.id)}
                  >
                    Delete
                  </Button>
                </div>
              </td>
            </tr>
          ))}
        </tbody>
      </Table>
      <Button href="/questions/create">Create Question</Button>
    </div>
  );
};

export default ReadQuestions;
