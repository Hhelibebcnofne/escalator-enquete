import { FC } from "react";

interface ReadUsersProps {
  users: User[];
}

export type User = {
  id: number;
  name: string;
};

const ReadUsers: FC<ReadUsersProps> = ({ users }) => {
  const handleDeleteUser = (id: number) => {
    fetch(`/users/${id}`, {
      method: "delete",
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
            <p>{user.name}</p>
          </div>
          <div>
            <a href={`/users/${user.id}`}>Update </a>
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
