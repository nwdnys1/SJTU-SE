package org.example.service.Impl.LLM;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.mockito.Mockito.*;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.stream.Stream;

import org.example.DAO.*;
import org.example.DTO.PageResponse;
import org.example.entity.*;
import org.example.service.Impl.OrderServiceImpl;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageImpl;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;

@ExtendWith(MockitoExtension.class)
class OrderServiceImplTestLLM {

	@Mock
	private OrderDAO orderDAO;
	@Mock
	private OrderItemDAO orderItemDAO;
	@Mock
	private CartItemDAO cartItemDAO;
	@Mock
	private UserDAO userDAO;
	@Mock
	private BookDAO bookDAO;

	@InjectMocks
	private OrderServiceImpl orderService;

	// ========== addOrder 测试 ==========
	static Stream<Arguments> provideAddOrderTestCases() {
		OrderRequest validRequest = new OrderRequest();
		validRequest.setUid(1);
		validRequest.setCids(Arrays.asList(1, 2));
		validRequest.setAddress("address");
		validRequest.setTel("123456");
		validRequest.setReceiver("receiver");

		return Stream.of(Arguments.of(validRequest, false, false, false, 200, "正常下单"),
				Arguments.of(validRequest, true, false, false, 400, "购物车项不存在"),
				Arguments.of(validRequest, false, true, false, 400, "购物车项不属于用户"),
				Arguments.of(validRequest, false, false, true, 400, "库存不足"));
	}

	// ========== deleteOrder 测试 ==========
	static Stream<Arguments> provideDeleteOrderTestCases() {
		return Stream.of(Arguments.of(1, true, false, 200, "正常删除"), Arguments.of(2, true, true, 403, "无权删除他人订单"),
				Arguments.of(3, false, false, 404, "订单不存在"));
	}

	@BeforeEach
	public void setup() {
		UserDetails user = new org.springframework.security.core.userdetails.User("testUser", "testPassword", true,
				true, true, true, Collections.singletonList((GrantedAuthority) () -> "ROLE_USER"));
		Authentication authentication = new UsernamePasswordAuthenticationToken(user, null, user.getAuthorities());
		SecurityContextHolder.getContext().setAuthentication(authentication);
	}

	// ========== getOrders 测试 ==========
	@Test
	void testGetOrdersLLM() {
		User mockUser = new User();
		mockUser.setId(1);
		when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

		List<Order> mockOrders = Arrays.asList(new Order(), new Order());
		when(orderDAO.getOrdersByUserId(1)).thenReturn(mockOrders);

		Result<List<Order>> result = orderService.getOrders();

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().size());
	}

	@ParameterizedTest
	@MethodSource("provideAddOrderTestCases")
	void testAddOrderLLM(OrderRequest request, boolean cartItemNotExist, boolean wrongUser, boolean outOfStock,
			int expectedStatus, String description) {
		User mockUser = new User();
		mockUser.setId(1);

		if (cartItemNotExist) {
			when(cartItemDAO.getCartItemById(anyInt())).thenReturn(null);
		} else if (wrongUser) {
			CartItem wrongUserItem = new CartItem();
			wrongUserItem.setUser(new User());
			wrongUserItem.getUser().setId(2);
			when(cartItemDAO.getCartItemById(anyInt())).thenReturn(wrongUserItem);
		} else if (outOfStock) {
			CartItem item = new CartItem();
			item.setUser(mockUser);
			item.setQuantity(10);
			Book book = new Book();
			book.setId(1);
			item.setBook(book);
			when(cartItemDAO.getCartItemById(anyInt())).thenReturn(item);

			BookDetails bookDetails = new BookDetails();
			bookDetails.setStock(5);
			when(bookDAO.getBookDetailsById(1)).thenReturn(bookDetails);
		} else {
			CartItem item1 = new CartItem();
			item1.setUser(mockUser);
			item1.setQuantity(2);
			Book book1 = new Book();
			book1.setId(1);
			item1.setBook(book1);

			CartItem item2 = new CartItem();
			item2.setUser(mockUser);
			item2.setQuantity(1);
			Book book2 = new Book();
			book2.setId(2);
			item2.setBook(book2);

			when(cartItemDAO.getCartItemById(1)).thenReturn(item1);
			when(cartItemDAO.getCartItemById(2)).thenReturn(item2);

			BookDetails bookDetails = new BookDetails();
			bookDetails.setStock(10);
			when(bookDAO.getBookDetailsById(anyInt())).thenReturn(bookDetails);

			doNothing().when(orderDAO).save(any(Order.class));
		}

		Result<Order> result = orderService.addOrder(request);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertNotNull(result.getData());
		}
	}

	@ParameterizedTest
	@MethodSource("provideDeleteOrderTestCases")
	void testDeleteOrderLLM(int id, boolean orderExists, boolean wrongUser, int expectedStatus, String description) {
		User mockUser = new User();
		mockUser.setId(1);
		if (orderExists)
			when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

		if (orderExists) {
			Order mockOrder = new Order();
			mockOrder.setUser(wrongUser ? new User() : mockUser);
			mockOrder.getUser().setId(wrongUser ? 2 : 1);
			when(orderDAO.getOrderById(id)).thenReturn(mockOrder);
			when(orderDAO.existsById(id)).thenReturn(true);
		} else {
			when(orderDAO.existsById(id)).thenReturn(false);
		}

		Result<Order> result = orderService.deleteOrder(id);

		assertEquals(expectedStatus, result.getCode());
	}

	// ========== getAllOrders 测试 ==========
	@Test
	void testGetAllOrdersLLM() {
		List<Order> mockOrders = Arrays.asList(new Order(), new Order());
		when(orderDAO.findAll()).thenReturn(mockOrders);

		Result<List<Order>> result = orderService.getAllOrders();

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().size());
	}

	// ========== searchAllOrders 测试 ==========
	static Stream<Arguments> provideSearchAllOrdersTestCases() {
		return Stream.of(Arguments.of("test", LocalDateTime.now().minusDays(1), LocalDateTime.now(), 1, 10),
				Arguments.of("", LocalDateTime.now().minusDays(1), LocalDateTime.now(), 1, 10));

	}
	@ParameterizedTest
	@MethodSource("provideSearchAllOrdersTestCases")
	void testSearchAllOrdersLLM(String keyword, LocalDateTime start, LocalDateTime end, int page, int pageSize) {
		Pageable pageable = PageRequest.of(page - 1, pageSize);
		Page<Order> mockPage = new PageImpl<>(Arrays.asList(new Order(), new Order()));

	    if(keyword.isEmpty())
	        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, pageable)).thenReturn(mockPage);
	    else
	        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(start, end, "%" + keyword + "%", pageable)).thenReturn(mockPage);
		Result<PageResponse<Order>> result = orderService.searchAllOrders(keyword, page, pageSize, start, end);

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().getTotal());
	}

	// ========== searchOrders 测试 ==========
	static Stream<Arguments> provideSearchOrdersTestCases() {
		return Stream.of(Arguments.of("test", LocalDateTime.now().minusDays(1), LocalDateTime.now(), 1, 10),
				Arguments.of("", LocalDateTime.now().minusDays(1), LocalDateTime.now(), 1, 10));
	}
	@ParameterizedTest
	@MethodSource("provideSearchOrdersTestCases")
	void testSearchOrdersLLM(String keyword, LocalDateTime start, LocalDateTime end, int page, int pageSize) {
		Pageable pageable = PageRequest.of(page - 1, pageSize);
		Page<Order> mockPage = new PageImpl<>(Arrays.asList(new Order(), new Order()));
		if(keyword.isEmpty())
			when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, 0, pageable)).thenReturn(mockPage);
		else
			when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(start, end, "%" + keyword + "%", 0, pageable)).thenReturn(mockPage);

		when(userDAO.getUserByUsername(anyString())).thenReturn(new User());


		Result<PageResponse<Order>> result = orderService.searchOrders(keyword, start, end, page, pageSize);

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().getTotal());
	}

	// ========== findCartItemById 测试 ==========
	static Stream<Arguments> provideFindCartItemByIdTestCases() {
		CartItem item1 = new CartItem();
		item1.setId(1);
		CartItem item2 = new CartItem();
		item2.setId(2);
		List<CartItem> cartItems = Arrays.asList(item1, item2);

		return Stream.of(Arguments.of(cartItems, 1, item1), Arguments.of(new ArrayList<>(), 1, null)
				);
	}
	@ParameterizedTest
	@MethodSource("provideFindCartItemByIdTestCases")
	void testFindCartItemByIdLLM(List<CartItem> cartItems, int cid, CartItem expected) {
		CartItem result = orderService.findCartItemById(cartItems, cid);
		assertEquals(expected, result);
	}
}