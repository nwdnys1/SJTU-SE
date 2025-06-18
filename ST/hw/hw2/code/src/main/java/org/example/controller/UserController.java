package org.example.controller;

import java.time.LocalDateTime;
import java.util.List;

import org.example.DTO.SpentInfo;
import org.example.DTO.PurchaseInfo;
import org.example.entity.RegisterRequest;
import org.example.entity.Result;
import org.example.entity.User;
import org.example.entity.UserProfile;
import org.example.service.Impl.MyUserDetailsService;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

@RestController
@RequestMapping("/api/user")
public class UserController {
	private final MyUserDetailsService service;
	public UserController(MyUserDetailsService service) {
		this.service = service;
	}
	@GetMapping("/check")
	public Result<String> check() {// 检查是否登录
		UserDetails user = (UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal();
		return Result.success("已登录");
	}
	@GetMapping("/get") // 获取当前登录用户的信息
	public Result<User> get() {
		UserDetails user = (UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal();
		return service.getUserByUsername(user.getUsername());
	}
	@GetMapping("/get/{uid}") // 根据用户名获取用户信息 一般是用于查看别人的信息
	public Result<User> get(@PathVariable int uid) {
		return service.getUserById(uid);
	}
	@GetMapping("/admin/list") // 获取所有用户信息
	public Result<List<User>> list() {
		return service.getAllUsers();
	}
	@PostMapping("/register")
	public Result<User> register(@RequestBody RegisterRequest request) {
		return service.addUser(request);
	}
	@PutMapping("/update")
	public Result<User> update(@RequestBody UserProfile request) {
		return service.updateUser(request);
	}
	@PutMapping("/admin/disable/{id}")
	public Result<User> disable(@PathVariable int id) {
		return service.disableUser(id);
	}
	@PutMapping("/admin/enable/{id}")
	public Result<User> enable(@PathVariable int id) {
		return service.enableUser(id);
	}
	@DeleteMapping("/admin/delete/{id}")
	public Result<User> delete(@PathVariable int id) {
		return service.deleteUser(id);
	}
	@PostMapping("/avatar")
	public Result<String> uploadAvatar(@RequestParam("avatar") MultipartFile file) {
		return service.updateAvatar(file);
	}
	@GetMapping("/admin/rank") // 获取用户的消费排名
	public Result<List<SpentInfo>> rank(
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime start,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime end,
			@RequestParam(defaultValue = "10") int nums) {
		return service.rank(start, end, nums);
	}
	@GetMapping("/statistics") // 获取用户的消费统计
	public Result<List<PurchaseInfo>> statistics(
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime start,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime end) {
		return service.statistics(start, end);
	}
}
