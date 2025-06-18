import React from "react";
import ReactDOM from "react-dom/client"; // Import createRoot from "react-dom/client" instead of "react-dom"

import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import HomePage from "./pages/home";
import SkillPage from "./pages/skill";
import PersonPage from "./pages/person";
import JobPage from "./pages/job";

const App = () => (
  <Router>
    <Routes>
      <Route path="/" element={<HomePage />} />
      <Route path="/skills" element={<SkillPage />} />
      <Route path="/jobs" element={<JobPage />} />
      <Route path="/person/:name" element={<PersonPage />} />
    </Routes>
  </Router>
);

ReactDOM.createRoot(document.getElementById("root")).render(<App />);
