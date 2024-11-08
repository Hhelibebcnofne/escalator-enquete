import { ChangeEvent, FC, useState } from "react";
import { getCsrfToken } from "../../utils/csrf";

const CreateUser: FC = () => {
  const [userName, setUserName] = useState("");
  const handleUserNameChange = (event: ChangeEvent<HTMLInputElement>) => {
    setUserName(event.target.value);
  };
  const csrfToken = getCsrfToken();
  const handleFormSubmit = (event: ChangeEvent<HTMLFormElement>) => {
    event.preventDefault();
    fetch(`/users`, {
      method: "post",
      headers: csrfToken
        ? {
            "Content-Type": "application/json",
            "X-CSRFToken": csrfToken,
          }
        : { "Content-Type": "application/json" },
      body: JSON.stringify({ name: userName }),
    }).then(() => {
      window.location.href = "/users";
    });
  };

  return (
    <div>
      <h1>Create User</h1>
      <form onSubmit={handleFormSubmit}>
        <input
          id="name"
          type="text"
          value={userName}
          onChange={handleUserNameChange}
          required={true}
        />
        <button type="submit">Create User</button>
      </form>
    </div>
  );
};

export default CreateUser;
