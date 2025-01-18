package org.example.backend.controller;

import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpSession;
import org.example.backend.entity.Result;
import org.example.backend.entity.User;
import org.example.backend.service.Impl.MyUserDetailsService;
import org.example.backend.service.TimerService;
import org.springframework.context.annotation.Scope;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.authentication.dao.DaoAuthenticationProvider;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.web.bind.annotation.*;


@RestController
@RequestMapping("/api/user")
@Scope("session")
public class LoginController {
    private final TimerService timerService;
    private final MyUserDetailsService myUserDetailsService;
    private final PasswordEncoder passwordEncoder;
    public LoginController (TimerService timerService, MyUserDetailsService myUserDetailsService, PasswordEncoder passwordEncoder) {
        this.timerService = timerService;
        this.myUserDetailsService = myUserDetailsService;
        this.passwordEncoder = passwordEncoder;
    }
    @PostMapping("/login")
    public Result<User> login(@RequestParam String username,@RequestParam String password,HttpServletRequest request) throws Exception {
        UsernamePasswordAuthenticationToken usernamePasswordAuthenticationToken = new UsernamePasswordAuthenticationToken(username, password);
        DaoAuthenticationProvider daoAuthenticationProvider = new DaoAuthenticationProvider(passwordEncoder);
        daoAuthenticationProvider.setUserDetailsService(myUserDetailsService);
        Authentication authenticate = daoAuthenticationProvider.authenticate(usernamePasswordAuthenticationToken);
        SecurityContextHolder.getContext().setAuthentication(authenticate);//设置上下文
        request.getSession().setAttribute("SPRING_SECURITY_CONTEXT", SecurityContextHolder.getContext());//设置session
        SecurityContextHolder.clearContext();//清除上下文
        timerService.start();
        return myUserDetailsService.getUserByUsername(username);
    }
    @GetMapping("/logout")
    public Result<String> logout(HttpServletRequest request) {
        HttpSession session = request.getSession();
        if(session.getAttribute("SPRING_SECURITY_CONTEXT") == null) return Result.success("未登录");
        session.invalidate();
        timerService.end();
        return Result.success("登出成功, 本次会话时长：" + timerService.getTime() + "秒");
    }

}
