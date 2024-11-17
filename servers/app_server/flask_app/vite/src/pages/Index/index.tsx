import "@/App.css";
import { Button } from "react-bootstrap";

function App() {
  return (
    <>
      <h1>Escalator Enquete</h1>
      <Button variant="secondary" className="m-2" href="/questions">
        Questions
      </Button>
      <Button variant="secondary" className="m-2" href="/sensing_counts">
        Sensing Counts
      </Button>
    </>
  );
}

export default App;
