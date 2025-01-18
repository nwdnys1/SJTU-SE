package org.example.backend.config;

import java.io.IOException;
import java.io.PrintWriter;

import org.example.backend.entity.Result;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.access.AccessDeniedException;
import org.springframework.security.authentication.AccountStatusException;
import org.springframework.security.authentication.AuthenticationCredentialsNotFoundException;
import org.springframework.security.authentication.BadCredentialsException;
import org.springframework.security.authentication.InsufficientAuthenticationException;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configurers.AbstractHttpConfigurer;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.AuthenticationException;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;

import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

@Configuration
public class SecurityConfig {
	@Bean
	public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
		return http.authorizeHttpRequests((requests) -> {
			requests.requestMatchers("/api/book/admin/**", "/api/user/admin/**").hasRole("admin")// 只有管理员才能访问
					.requestMatchers("api/user/update", "/api/user/avatar").authenticated()// 登录后才能访问
					.requestMatchers("/api/book/**", "/api/comment/list/**", "/api/user/**", "/api/tag/**", "/image/**","/graphql")
					.permitAll().anyRequest().authenticated();
		}).formLogin(conf -> {
			// conf.loginProcessingUrl("/api/login");
			conf.successHandler(this::handleProcess);
			conf.failureHandler(this::handleProcess);
			conf.permitAll();
		}).csrf(AbstractHttpConfigurer::disable).rememberMe(conf -> {
			conf.alwaysRemember(true);
		}).exceptionHandling(conf -> {
			conf.accessDeniedHandler(this::handleProcess);
			conf.authenticationEntryPoint(this::handleProcess);
		}).logout(conf -> {
			// conf.logoutUrl("/api/user/logout");
			conf.logoutSuccessHandler(this::handleProcess);
		}).build();
	}
	@Bean
	public PasswordEncoder passwordEncoder() {// 密码编码器
		return new BCryptPasswordEncoder();
	}
	// @Bean
	// public MyAuthenticationProvider authenticationProvider() {
	// return new MyAuthenticationProvider(repository, passwordEncoder());
	// }
	// @Bean
	// public AuthenticationManager authenticationManager(){
	// return new
	// ProviderManager(Collections.singletonList(authenticationProvider()));
	// }

	private void handleProcess(HttpServletRequest request, HttpServletResponse response,
			Object exceptionOrAuthentication) throws IOException {
		response.setContentType("application/json;charset=utf-8");
		PrintWriter writer = response.getWriter();
		if (exceptionOrAuthentication instanceof AuthenticationException authException) {
			// 如果是认证异常，则判断具体是用户名密码错误还是未登录
			if (authException instanceof BadCredentialsException) {
				// 用户名密码错误
				writer.write(Result.error(401, "用户名或密码错误").asJsonString());
				response.setStatus(401);
			} else if (authException instanceof AccountStatusException) {
				// 账号被禁用
				writer.write(Result.error(401, "您的账号已被禁用！").asJsonString());
				response.setStatus(401);
			} else if (authException instanceof InsufficientAuthenticationException
					|| authException instanceof AuthenticationCredentialsNotFoundException) {
				// 未登录
				writer.write(Result.error(401, "请先登录").asJsonString());
				response.setStatus(401);
			} else {
				// 其他认证异常
				writer.write(Result.error(401, authException.getMessage()).asJsonString());
				response.setStatus(401);
			}
		} else if (exceptionOrAuthentication instanceof AccessDeniedException) {
			// 权限不足
			writer.write(Result.error(403, "您没有权限访问").asJsonString());
			response.setStatus(403);
		} else if (exceptionOrAuthentication instanceof Authentication) {
			// 身份验证成功
			writer.write(Result.success((Authentication) exceptionOrAuthentication).asJsonString());
		} else {
			// 其他情况，返回通用错误信息
			writer.write(Result.error(500, "服务器内部错误").asJsonString());
			response.setStatus(500);
		}

	}

}
