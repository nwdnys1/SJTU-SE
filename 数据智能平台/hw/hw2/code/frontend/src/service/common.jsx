//自定义处理响应的函数，如果响应成功，返回响应的数据，否则抛出错误信息
export async function handleResponse(res) {
  if (res.code === 200) {
    return res.data;
  } else {
    throw res.message;
  }
}

//发送GET请求，获取json数据
export async function getJson(url) {
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

//发送PUT请求，修改数据
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

//发送DELETE请求，删除数据
export async function del(url, data) {
  let opts = {
    method: "DELETE",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    },
    credentials: "include",
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

//发送POST请求，提交数据，这里的特殊数据格式是x-www-form-urlencoded，为了和Spring Security兼容
export async function post_x_www_form_urlencoded(url, data) {
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
  let cookies = res.headers.get("Set-Cookie");
  if (cookies) {
    console.log("cookies", cookies);
    document.cookie = cookies;
  }
  return await res.json().then(handleResponse);
}

//发送POST请求，提交数据
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

//发送POST请求，提交数据
export async function postWithoutHandle(url, data) {
  let opts = {
    method: "POST",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    },
    credentials: "include",
  };
  let res = await fetch(url, opts);
  let resdata = await res.json();
  console.log(resdata);
  return await resdata;
}

//发送POST请求，提交数据
export async function postWithoutJsonParse(url, data) {
  console.log(JSON.stringify(data));
  let opts = {
    method: "POST",
    body: JSON.stringify(data),
    headers: {
      "Content-Type": "application/json",
    },
    credentials: "include",
  };
  let res = await fetch(url, opts);
  return await res;
}

//提交带图片的表单数据
export async function postFormData(url, formData) {
  let opts = {
    method: "POST",
    body: formData,
    credentials: "include",
  };
  let res = await fetch(url, opts);
  return await res.json().then(handleResponse);
}

//定义后端的url，是本地的后端服务
export const BASEURL = "http://10.119.15.62:5000";

//定义服务的url前缀
export const PREFIX = `${BASEURL}/api`;
