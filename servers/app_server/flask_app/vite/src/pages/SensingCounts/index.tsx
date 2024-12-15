import React, { useEffect, useRef } from "react";
import { Button } from "react-bootstrap";
import type { Statistics } from "../../utils/types";
import { Bar } from "react-chartjs-2";
import { Chart, registerables } from "chart.js";

Chart.register(...registerables);
interface SensingCountsProps {
  statistics: Statistics;
}

const SensingCounts: React.FC<SensingCountsProps> = ({ statistics }) => {
  const chartRef = useRef<Chart | null>(null);

  useEffect(() => {
    const currentChartRef = chartRef.current;
    return () => {
      if (currentChartRef) {
        currentChartRef.destroy();
      }
    };
  }, []);

  return (
    <div className="container">
      <h1>これまでの統計結果一覧</h1>
      {Object.values(statistics).map((statistic) => (
        <Bar
          key={statistic.sentence}
          data={{
            labels: [statistic.sentence],
            datasets: [
              {
                label: statistic.optionA,
                data: [statistic.optionA_count],
                backgroundColor: "rgba(255, 99, 132, 0.2)",
                borderColor: "rgba(255, 99, 132, 1)",
                borderWidth: 1,
              },
              {
                label: statistic.optionB,
                data: [statistic.optionB_count],
                backgroundColor: "rgba(54, 162, 235, 0.2)",
                borderColor: "rgba(54, 162, 235, 1)",
                borderWidth: 1,
              },
            ],
          }}
          options={{
            indexAxis: "y",
            scales: {
              y: {
                beginAtZero: true,
              },
            },
          }}
        />
      ))}
      <Button variant="secondary" className="m-2" href="/">
        Back
      </Button>
    </div>
  );
};

export default SensingCounts;
