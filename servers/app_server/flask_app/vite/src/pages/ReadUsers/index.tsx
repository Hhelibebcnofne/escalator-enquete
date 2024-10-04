import { FC } from "react";

interface ReadUsersProps {
  users: User[];
}
type User = {
  id: number;
  name: string;
};

const ReadUsers: FC<ReadUsersProps> = ({ users }) => {
  return (
    <div>
      <h1>Read Users</h1>
      {users.map((user) => (
        <div key={user.id}>
          <p>{user.name}</p>
        </div>
      ))}
    </div>
  );
};

export default ReadUsers;
