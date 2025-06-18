import { useEffect, useState } from "react";
import { BasicLayout } from "./layout";
import { Bar } from "@ant-design/plots";
import { getStats } from "../service/service";
const Stats = () => {
  const [data, setData] = useState([]);

  const config = (data, title) => ({
    data: data,
    xField: "id",
    yField: "count",
    marginTop: 50,
    height: 400,

    label: {
      text: (item) => `${item.count}`, // 显示的文本内容
      style: {
        textAnchor: (d) => (+d.frequency > 0.008 ? "right" : "start"),
        fill: (d) => (+d.frequency > 0.008 ? "#fff" : "#000"),
        dx: (d) => (+d.frequency > 0.008 ? -5 : 5),
      },
    },
    style: {
      // 圆角样式
      radiusTopLeft: 10,
      radiusTopRight: 10,
      maxWidth: 20,
    },

    title: title,
  });
  useEffect(() => {
    getStats().then((res) => {
      setData(res);
    });
  }, []);
  useEffect(() => {}, [data]); //
  return (
    <BasicLayout>
      <Bar {...config(data.skills, "候选人掌握哪些技能最多")} />
      <Bar {...config(data.jobs, "候选人拥有哪些工作经历最多？")} />
      <Bar {...config(data.countries, "候选人来自于哪些国家最多？")} />
      <Bar {...config(data.languages, "候选人使用哪种语言最多？")} />
      {/* <Bar {...config(data.locations, "候选人在哪些地区工作最多？")} /> */}
    </BasicLayout>
  );
};

export default Stats;
