import { FC } from "react";
import { User } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";

interface ReadUserProps {
  user: User;
}

const ReadUser: FC<ReadUserProps> = ({ user }) => {
  const csrfToken = getCsrfToken();
  const handleDeleteUser = (id: number) => {
    fetch(`/users/${id}`, {
      method: "delete",
      headers: csrfToken ? { "X-CSRF-Token": csrfToken } : {},
    }).then(() => {
      window.location.href = "/users";
    });
  };
  return (
    <>
      <h1>Read User</h1>
      <div key={user.id}>
        <p>
          {user.id}
          {user.name}
        </p>
      </div>
      <div>
        <a href={`/users/edit/${user.id}`}>Update </a>
        <a
          onClick={() => handleDeleteUser(user.id)}
          style={{ cursor: "pointer" }}
        >
          Delete
        </a>
      </div>
    </>
  );
};

export default ReadUser;
