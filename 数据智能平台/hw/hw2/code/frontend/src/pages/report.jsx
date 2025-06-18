import React, { useState, useEffect } from "react";
import BasicLayout from "../layout/basicLayout";
import ReportList from "../components/reportList";

export default function ReportPage() {
  return (
    <BasicLayout>
      <ReportList />
    </BasicLayout>
  );
}
