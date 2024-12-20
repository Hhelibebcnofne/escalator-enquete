import { FC } from "react";
import { Users } from "../../utils/types";
import { getCsrfToken } from "../../utils/csrf";

interface ReadUsersProps {
  users: Users;
}

const ReadUsers: FC<ReadUsersProps> = ({ users }) => {
  const csrfToken = getCsrfToken();
  const handleDeleteUser = (id: number) => {
    fetch(`/users/${id}`, {
      method: "delete",
      headers: csrfToken
        ? {
            "X-CSRF-Token": csrfToken,
          }
        : {},
    }).then(() => {
      window.location.href = "/users";
    });
  };
  return (
    <div>
      <h1>Read Users</h1>
      {users.map((user) => (
        <>
          <div key={user.id}>
            <p>
              {user.id} {user.name}
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
      ))}
      <a href="/users/create">Create User</a>
    </div>
  );
};

export default ReadUsers;
