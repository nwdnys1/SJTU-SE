import React, { useEffect, useState } from "react";

const Blur = () => {
  return (
    <div
      style={{
        position: "absolute",
        bottom: 0,
        width: "100%",
        height: "40px",
        background: `rgba(255, 255, 255, 0)`,
        backdropFilter: "blur(2px)",
        zIndex: 1,
      }}
    ></div>
  );
};

export default Blur;
