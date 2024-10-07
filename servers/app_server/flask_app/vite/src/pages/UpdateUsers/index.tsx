import { ChangeEvent, FC, useState } from "react";
import { User } from "../ReadUsers/index";

interface UpdateUsersProps {
  user: User;
}

const Update: FC<UpdateUsersProps> = ({ user }) => {
  const [userName, setUserName] = useState(user.name);
  const handleUserNameChange = (event: ChangeEvent<HTMLInputElement>) => {
    setUserName(event.target.value);
  };
  const handleFormSubmit = (event: ChangeEvent<HTMLFormElement>) => {
    event.preventDefault();
    fetch(`/users/${user.id}`, {
      method: "put",
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
      <h1>Update User</h1>
      <form onSubmit={handleFormSubmit}>
        <input
          id="name"
          type="text"
          value={userName}
          onChange={handleUserNameChange}
        />
        <button type="submit">Update User</button>
      </form>
    </div>
  );
};

export default Update;
