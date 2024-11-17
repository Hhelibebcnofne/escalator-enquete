import React from "react";
import { Button, Table } from "react-bootstrap";
import type { Statistics } from "../../utils/types";

interface SensingCountsProps {
  statistics: Statistics;
}

const SensingCounts: React.FC<SensingCountsProps> = ({ statistics }) => {
  console.log(statistics);
  return (
    <div className="container">
      <h1>Sensing Counts</h1>
      <Table striped bordered hover>
        <thead>
          <tr>
            <th>Sentence</th>
            <th>Option A</th>
            <th>Option B</th>
            <th>Option A Count</th>
            <th>Option B Count</th>
            <th>Error Count</th>
          </tr>
        </thead>
        <tbody>
          {Object.values(statistics).map((statistic) => (
            <tr key={statistic.sentence}>
              <td>{statistic.sentence}</td>
              <td>{statistic.optionA}</td>
              <td>{statistic.optionB}</td>
              <td>{statistic.optionA_count}</td>
              <td>{statistic.optionB_count}</td>
              <td>{statistic.error_count}</td>
            </tr>
          ))}
        </tbody>
      </Table>
      <Button variant="secondary" className="m-2" href="/">
        Back
      </Button>
    </div>
  );
};

export default SensingCounts;
