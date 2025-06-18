import { BASEURL, get } from "./requestService";

export async function getSkills() {
  const url = `${BASEURL}/skills`;
  let result;
  result = await get(url);
  return result;
}
export async function filterBySkills(skills) {
  const url = `${BASEURL}/has_skills?skills=${JSON.stringify(skills).replace(
    /\+/g,
    "%2B"
  )}`;
  let result;
  result = await get(url);
  return result;
}
export async function getJobs() {
  const url = `${BASEURL}/jobs`;
  let result;
  result = await get(url);
  return result;
}
export async function filterByJobs(jobs) {
  const url = `${BASEURL}/worked_as?jobtitles=${JSON.stringify(jobs)}`;
  let result;
  result = await get(url);
  return result;
}
export async function getPerson(name) {
  const url = `${BASEURL}/person/${name}`;
  let result;
  result = await get(url);
  return result;
}

export async function getStats() {
  const url = `${BASEURL}/count`;
  let result;
  result = await get(url);
  return result;
}
