import { ChangeEvent, FC, useState } from "react";
import { getCsrfToken } from "../../utils/csrf";
import { Button, Form } from "react-bootstrap";

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
    <div className="container">
      <h1>Create Question</h1>
      <Form onSubmit={handleFormSubmit}>
        <Form.Group className="mb-3">
          <Form.Label htmlFor="questionText">Question Text</Form.Label>
          <Form.Control
            id="questionText"
            name="questionText"
            type="text"
            value={sentence}
            onChange={handleSentenceChange}
            required={true}
          />
        </Form.Group>
        <Form.Group className="mb-3">
          <Form.Label htmlFor="optionAText">Option A Text</Form.Label>
          <Form.Control
            id="optionAText"
            name="optionAText"
            type="text"
            value={optionAText}
            onChange={handleOptionATextChange}
            required={true}
          />
        </Form.Group>
        <Form.Group className="mb-3">
          <Form.Label htmlFor="optionBText">Option B Text</Form.Label>
          <Form.Control
            id="optionBText"
            name="optionBText"
            type="text"
            value={optionBText}
            onChange={handleOptionBTextChange}
            required={true}
          />
        </Form.Group>

        <Button href="/questions" className="me-2">
          Back
        </Button>
        <Button type="submit">Create Question</Button>
      </Form>
    </div>
  );
};

export default CreateQuestion;
