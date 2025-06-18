import { createBrowserRouter, RouterProvider } from "react-router-dom";
import "./css/global.css";
import React from "react";
import DevicePage from "./pages/device";
import ErrorPage from "./pages/errorpage";
import TopicPage from "./pages/topic";
import SystemPage from "./pages/system";
import PredictPage from "./pages/predict";
import PcaPage from "./pages/pca";
import ReportPage from "./pages/report";

const router = createBrowserRouter([
  {
    path: "/",
    element: <DevicePage />,
    errorElement: <ErrorPage />,
  },
  {
    path: "/topic",
    element: <TopicPage />,
    errorElement: <ErrorPage />,
  },
  {
    path: "/system",
    element: <SystemPage />,
    errorElement: <ErrorPage />,
  },
  {
    path: "/predict",
    element: <PredictPage />,
    errorElement: <ErrorPage />,
  },
  {
    path: "/pca",
    element: <PcaPage />,
    errorElement: <ErrorPage />,
  },
  {
    path: "/report",
    element: <ReportPage />,
    errorElement: <ErrorPage />,
  },
]);

export default function App() {
  return <RouterProvider router={router} />;
}
