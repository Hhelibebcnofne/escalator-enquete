import { FC } from "react";

const Delete: FC = () => {
  return (
    <div>
      <h1>Delete User</h1>
      <form method="post" action="add">
        <input type="text" placeholder="Name" />
        <button type="submit">Delete User</button>
      </form>
    </div>
  );
};

export default Delete;
