package org.example.backend.entity;

import com.alibaba.fastjson2.JSONObject;
import com.alibaba.fastjson2.JSONWriter;
import lombok.Data;

@Data
public class Result<T> {
    int code;
    T data;
    String message;

    private Result(int code, T data, String message) {
        this.code = code;
        this.message = message;
        this.data = data;
    }

    public static <T> Result<T> success(T data) {
        return new Result<>(200, data, "请求成功!");
    }

    public static <T> Result<T> error(int code, String message) {
        return new Result<>(code, null, message);
    }

    public String asJsonString() {
        return JSONObject.from(this, JSONWriter.Feature.WriteNulls).toString();
    }
}
