package org.example.controller;

import java.time.LocalDateTime;
import java.util.List;

import org.example.DAO.UserDAO;
import org.example.DTO.PageResponse;
import org.example.entity.Order;
import org.example.entity.OrderRequest;
import org.example.entity.Result;
import org.example.entity.User;
import org.example.service.OrderService;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.web.bind.annotation.*;

import com.alibaba.fastjson2.JSON;

@RestController
@RequestMapping("/api/order")
public class OrderController {
	private final OrderService service;
	private final KafkaTemplate<String, String> kafkaTemplate;
	private final UserDAO userDAO;
	public OrderController(OrderService service, KafkaTemplate<String, String> kafkaTemplate, UserDAO userDAO) {
		this.service = service;
		this.kafkaTemplate = kafkaTemplate;
		this.userDAO = userDAO;
	}
	public User getUser() {// 从数据库里查询id
		String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal())
				.getUsername();
		return userDAO.getUserByUsername(username);
	}
	@GetMapping("/list")
	public Result<List<Order>> getOrders() {
		return service.getOrders();
	}
	@GetMapping("/admin/list")
	public Result<List<Order>> getAllOrders() {
		return service.getAllOrders();
	}
	@PostMapping("/add")
	public Result<String> addOrder(@RequestBody OrderRequest request) {
		// System.out.println("控制层接收到的订单信息：" + request);
		request.setUid(getUser().getId());// 由于kafka消息处理时没有session上下文，所以需要在这里鉴权
		kafkaTemplate.send("AddOrder", JSON.toJSONString(request));// 发送订单信息到kafka 序列化为json
		return Result.success("订单已提交，等待处理");
	}
	@DeleteMapping("/delete/{id}")
	public Result<Order> deleteOrder(@PathVariable int id) {
		return service.deleteOrder(id);
	}
	@GetMapping("/search")
	public Result<PageResponse<Order>> searchOrders(@RequestParam String keyword,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime start,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime end,
			@RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10") int pageSize) {
		return service.searchOrders(keyword, start, end, page, pageSize);
	}
	@GetMapping("/admin/search")
	public Result<PageResponse<Order>> searchAllOrdersByBookTitle(@RequestParam String keyword,
			@RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10") int pageSize,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime start,
			@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime end) {
		return service.searchAllOrders(keyword, page, pageSize, start, end);
	}
}
