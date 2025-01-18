export const BASEURL = import.meta.env.VITE_BASEURL;
console.log(BASEURL);
export const PREFIX = `${BASEURL}/api`;

export async function handleResponse(res) {
  if (res.code === undefined) throw JSON.stringify(res); //如果没有code字段说明是非标准格式的返回 服务器内部错误
  if (res.code === 200) {
    return res.data;
  } else {
    throw res.message;
  }
}

export async function get(url) {
  let opts = {
    method: "GET",
    headers: {
      "Content-Type": "application/json",
    },
    credentials: "include",
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
    credentials: "include",
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
    credentials: "include",
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
    credentials: "include",
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
    credentials: "include",
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
    credentials: "include",
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}
