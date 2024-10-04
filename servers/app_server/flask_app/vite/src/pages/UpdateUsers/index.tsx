import { FC } from "react";

const Update: FC = () => {
  return (
    <div>
      <h1>Update User</h1>
      <form method="post" action="add">
        <input type="text" placeholder="Name" />
        <button type="submit">Update User</button>
      </form>
    </div>
  );
};

export default Update;
