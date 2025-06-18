import { Graph } from "@antv/g6";
import { useEffect, useRef } from "react";

export default function G6graph({ data }) {
  const containerRef = useRef(null);

  useEffect(() => {
    const graph = new Graph({
      autoResize: true,
      container: containerRef.current,
      data: data,
      layout: {
        // 指定要使用的布局算法
        type: "force",
        // 布局算法的配置项
        gravity: 0,
        
        // ...
      },
      node: {
        style: {
          label: true,
          labelText: (obj) => obj.id,
        },
      },
    });

    graph.render();
  }, [data]);

  return (
    <div
      ref={containerRef}
      style={{
        width: "100%",
        height: "100vh",
        border: "1px solid #ccc",
      }}
    />
  );
}
