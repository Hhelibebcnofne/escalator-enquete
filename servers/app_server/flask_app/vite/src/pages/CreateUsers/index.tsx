import { ChangeEvent, FC, useState } from "react";

const Create: FC = () => {
  const [userName, setUserName] = useState("");
  const handleUserNameChange = (event: ChangeEvent<HTMLInputElement>) => {
    setUserName(event.target.value);
  };
  const handleFormSubmit = (event: ChangeEvent<HTMLFormElement>) => {
    event.preventDefault();
    fetch(`/users`, {
      method: "post",
      headers: {
        "Content-Type": "application/json",
      },
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
        />
        <button type="submit">Create User</button>
      </form>
    </div>
  );
};

export default Create;
