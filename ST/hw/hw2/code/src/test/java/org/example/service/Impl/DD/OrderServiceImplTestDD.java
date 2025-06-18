package org.example.service.Impl.DD;

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
import org.springframework.data.domain.*;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContext;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;

import java.time.LocalDateTime;
import java.util.*;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class OrderServiceImplTestDD {

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
    @Mock
    private SecurityContext securityContext;
    @Mock
    private Authentication authentication;

    @InjectMocks
    private OrderServiceImpl orderService;

    @BeforeEach
    public void setup() {
        // 设置测试用的认证信息
        UserDetails user = new org.springframework.security.core.userdetails.User(
                "testUser", "testPassword", true, true, true, true,
                Collections.singletonList((GrantedAuthority) () -> "ROLE_USER"));
        Authentication authentication = new UsernamePasswordAuthenticationToken(
                user, null, user.getAuthorities());
        SecurityContextHolder.getContext().setAuthentication(authentication);
    }

    // ========== getOrders 测试 ==========
    @Test
    void testGetOrders() {
        User mockUser = new User();
        mockUser.setId(1);
        when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

        List<Order> mockOrders = Arrays.asList(new Order(), new Order());
        when(orderDAO.getOrdersByUserId(1)).thenReturn(mockOrders);

        Result<List<Order>> result = orderService.getOrders();

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
    }

    // ========== addOrder 测试 ==========
    static Stream<Arguments> provideAddOrderTestCases() {
        OrderRequest validRequest = new OrderRequest();
        validRequest.setUid(1);
        validRequest.setCids(Arrays.asList(1, 2));
        validRequest.setAddress("address");
        validRequest.setTel("123456");
        validRequest.setReceiver("receiver");

        OrderRequest invalidCartItemRequest = new OrderRequest();
        invalidCartItemRequest.setUid(1);
        invalidCartItemRequest.setCids(Arrays.asList(99));

        OrderRequest wrongUserCartItemRequest = new OrderRequest();
        wrongUserCartItemRequest.setUid(1);
        wrongUserCartItemRequest.setCids(Arrays.asList(2));

        OrderRequest outOfStockRequest = new OrderRequest();
        outOfStockRequest.setUid(1);
        outOfStockRequest.setCids(Arrays.asList(3));

        return Stream.of(
                Arguments.of(validRequest, false, false, false, 200, "正常下单"),
                Arguments.of(invalidCartItemRequest, true, false, false, 400, "购物车项不存在"),
                Arguments.of(wrongUserCartItemRequest, false, true, false, 400, "购物车项不属于用户"),
                Arguments.of(outOfStockRequest, false, false, true, 400, "库存不足")
        );
    }

    @ParameterizedTest
    @MethodSource("provideAddOrderTestCases")
    void testAddOrder(OrderRequest request, boolean cartItemNotExist, boolean wrongUser, boolean outOfStock, int expectedStatus, String description) {

        User mockUser = new User();
        mockUser.setId(1);
        //when(userDAO.getUserByUsername("user1")).thenReturn(mockUser);

        if (cartItemNotExist) {
            when(cartItemDAO.getCartItemById(99)).thenReturn(null);
        } else if (wrongUser) {
            CartItem wrongUserItem = new CartItem();
            wrongUserItem.setId(2);
            wrongUserItem.setUser(new User());
            wrongUserItem.getUser().setId(2); // 不属于当前用户
            when(cartItemDAO.getCartItemById(2)).thenReturn(wrongUserItem);
        } else if (outOfStock) {
            CartItem outOfStockItem = new CartItem();
            outOfStockItem.setId(3);
            outOfStockItem.setUser(mockUser);
            outOfStockItem.setQuantity(10);
            Book book = new Book();
            book.setId(1);
            outOfStockItem.setBook(book);
            when(cartItemDAO.getCartItemById(3)).thenReturn(outOfStockItem);

            BookDetails bookDetails = new BookDetails();
            bookDetails.setStock(5); // 库存不足
            when(bookDAO.getBookDetailsById(1)).thenReturn(bookDetails);
        } else {
            // 正常情况
            CartItem item1 = new CartItem();
            item1.setId(1);
            item1.setUser(mockUser);
            item1.setQuantity(2);
            Book book1 = new Book();
            book1.setId(1);
            item1.setBook(book1);

            CartItem item2 = new CartItem();
            item2.setId(2);
            item2.setUser(mockUser);
            item2.setQuantity(1);
            Book book2 = new Book();
            book2.setId(2);
            item2.setBook(book2);

            when(cartItemDAO.getCartItemById(1)).thenReturn(item1);
            when(cartItemDAO.getCartItemById(2)).thenReturn(item2);

            BookDetails bookDetails1 = new BookDetails();
            bookDetails1.setStock(10);
            BookDetails bookDetails2 = new BookDetails();
            bookDetails2.setStock(5);
            when(bookDAO.getBookDetailsById(1)).thenReturn(bookDetails1);
            when(bookDAO.getBookDetailsById(2)).thenReturn(bookDetails2);

            doNothing().when(orderDAO).save(any(Order.class));
        }

        Result<Order> result = orderService.addOrder(request);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertNotNull(result.getData());
            assertEquals(request.getAddress(), result.getData().getAddress());
        }
    }

    // ========== deleteOrder 测试 ==========
    static Stream<Arguments> provideDeleteOrderTestCases() {
        return Stream.of(
                Arguments.of(1, true, false, 200, "正常删除"),
                Arguments.of(2, true, true, 403, "无权删除他人订单"),
                Arguments.of(3, false, false, 404, "订单不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideDeleteOrderTestCases")
    void testDeleteOrder(int id, boolean orderExists, boolean wrongUser, int expectedStatus, String description) {
        User mockUser = new User();
        mockUser.setId(1);
        if (orderExists)        when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

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
    void testGetAllOrders() {
        List<Order> mockOrders = Arrays.asList(new Order(), new Order());
        when(orderDAO.findAll()).thenReturn(mockOrders);

        Result<List<Order>> result = orderService.getAllOrders();

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
    }

    // ========== searchAllOrders 测试 ==========
    static Stream<Arguments> provideSearchAllOrdersTestCases() {
        return Stream.of(
                Arguments.of("", 200, "无关键词搜索"),
                Arguments.of("book", 200, "有关键词搜索")
        );
    }

    @ParameterizedTest
    @MethodSource("provideSearchAllOrdersTestCases")
    void testSearchAllOrders(String keyword, int expectedStatus, String description) {
        int page = 1;
        int pageSize = 10;
        LocalDateTime start = LocalDateTime.now().minusDays(1);
        LocalDateTime end = LocalDateTime.now();

        Page<Order> mockPage = new PageImpl<>(Arrays.asList(new Order(), new Order()));
        if (keyword.isEmpty()) {
            when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, PageRequest.of(page - 1, pageSize)))
                    .thenReturn(mockPage);
        } else {
            when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(
                    start, end, "%" + keyword + "%", PageRequest.of(page - 1, pageSize)))
                    .thenReturn(mockPage);
        }

        Result<PageResponse<Order>> result = orderService.searchAllOrders(keyword, page, pageSize, start, end);

        assertEquals(expectedStatus, result.getCode());
        assertEquals(2, result.getData().getTotal());
    }

    // ========== searchOrders 测试 ==========
    static Stream<Arguments> provideSearchOrdersTestCases() {
        return Stream.of(
                Arguments.of("", 200, "无关键词搜索"),
                Arguments.of("book", 200, "有关键词搜索")
        );
    }

    @ParameterizedTest
    @MethodSource("provideSearchOrdersTestCases")
    void testSearchOrders(String keyword, int expectedStatus, String description) {
        User mockUser = new User();
        mockUser.setId(1);
        when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

        int page = 1;
        int pageSize = 10;
        LocalDateTime start = LocalDateTime.now().minusDays(1);
        LocalDateTime end = LocalDateTime.now();

        Page<Order> mockPage = new PageImpl<>(Arrays.asList(new Order(), new Order()));
        if (keyword.isEmpty()) {
            when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(
                    start, end, 1, PageRequest.of(page - 1, pageSize)))
                    .thenReturn(mockPage);
        } else {
            when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(
                    start, end, "%" + keyword + "%", 1, PageRequest.of(page - 1, pageSize)))
                    .thenReturn(mockPage);
        }

        Result<PageResponse<Order>> result = orderService.searchOrders(keyword, start, end, page, pageSize);

        assertEquals(expectedStatus, result.getCode());
        assertEquals(2, result.getData().getTotal());
    }

    // ========== findCartItemById 测试 ==========
    @Test
    void testFindCartItemById() {
        List<CartItem> cartItems = new ArrayList<>();
        CartItem item1 = new CartItem();
        item1.setId(1);
        CartItem item2 = new CartItem();
        item2.setId(2);
        cartItems.add(item1);
        cartItems.add(item2);

        CartItem result = orderService.findCartItemById(cartItems, 2);

        assertNotNull(result);
        assertEquals(2, result.getId());
    }

    @Test
    void testFindCartItemByIdNotFound() {
        List<CartItem> cartItems = new ArrayList<>();
        CartItem item1 = new CartItem();
        item1.setId(1);
        cartItems.add(item1);

        CartItem result = orderService.findCartItemById(cartItems, 2);

        assertNull(result);
    }
}