import "@/App.css";
import { Button } from "react-bootstrap";

function App() {
  return (
    <div className="container">
      <h1>Escalator Enquete</h1>
      <Button variant="secondary" className="m-2" href="/questions">
        Questions
      </Button>
      <Button variant="secondary" className="m-2" href="/sensing_counts">
        Sensing Counts
      </Button>
    </div>
  );
}

export default App;
