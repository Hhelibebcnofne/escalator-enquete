import { FC } from "react";

const Create: FC = () => {
  return (
    <div>
      <h1>Create User</h1>
      <form method="post" action="create">
        <input type="text" placeholder="Name" />
        <button type="submit">Create User</button>
      </form>
    </div>
  );
};

export default Create;
