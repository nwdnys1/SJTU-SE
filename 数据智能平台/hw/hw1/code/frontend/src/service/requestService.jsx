export const BASEURL = "http://localhost:5000";

export async function handleResponse(res) {
  return res;
}

export async function get(url) {
  let opts = {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

export async function post(url, data) {
  let opts = {
    method: "POST",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

export async function postText(url, data) {
  let opts = {
    method: "POST",
    body: data,
    headers: {
      "Content-Type": "text/plain",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

export async function postUrlencoded(url, data) {
  let formData = new URLSearchParams();
  for (let key in data) {
    formData.append(key, data[key]);
  }
  let opts = {
    method: "POST",
    body: formData,
    headers: {
      "Content-Type": "application/x-www-form-urlencoded",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

export async function Delete(url) {
  let opts = {
    method: "DELETE",
    headers: {
      "Content-Type": "application/json",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

export async function put(url, data) {
  let opts = {
    method: "PUT",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    },
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}
