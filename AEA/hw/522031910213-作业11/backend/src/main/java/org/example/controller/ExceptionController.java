package org.example.controller;

import org.example.entity.Result;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.security.authentication.AccountStatusException;
import org.springframework.security.authentication.AuthenticationCredentialsNotFoundException;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.InsufficientAuthenticationException;
import org.springframework.security.core.AuthenticationException;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestController;

@RestController
@ControllerAdvice
public class ExceptionController {
    @ExceptionHandler(Exception.class)
    public Result<String> error(Exception e){
        if (e instanceof AuthenticationException authException) {
            // 如果是认证异常，则判断具体是用户名密码错误还是未登录
            if (authException instanceof BadCredentialsException) {
                // 用户名密码错误
                return Result.error(401, "用户名或密码错误！");
            } else if (authException instanceof AccountStatusException) {
                // 账号被禁用
                return Result.error(401, "账号被禁用！");
            } else if(authException instanceof InsufficientAuthenticationException || authException instanceof AuthenticationCredentialsNotFoundException) {
                // 未登录
                return Result.error(401, "未登录！");
            } else {
                // 其他认证异常
                return Result.error(401, e.getMessage());
            }
        } else if (e instanceof AccessDeniedException) {
            // 权限不足
            return Result.error(403, "权限不足！");
        } else {
            // 其他情况，返回通用错误信息
            return Result.error(500, e.getMessage());
        }
    }
}
