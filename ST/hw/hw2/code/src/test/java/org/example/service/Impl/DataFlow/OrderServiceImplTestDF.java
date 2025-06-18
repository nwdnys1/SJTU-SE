package org.example.service.Impl.DataFlow;

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
class OrderServiceImplTestDF {

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
    void testGetOrdersDataFlow() {

        User mockUser = new User();
        mockUser.setId(1);
        when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

        Order order1 = new Order();
        order1.setId(1);
        Order order2 = new Order();
        order2.setId(2);
        List<Order> mockOrders = Arrays.asList(order1, order2);
        when(orderDAO.getOrdersByUserId(1)).thenReturn(mockOrders);

        Result<List<Order>> result = orderService.getOrders();

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(1, result.getData().get(0).getId());
        assertEquals(2, result.getData().get(1).getId());
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
                Arguments.of(validRequest, false, false, false, 200, "请求成功!"),
                Arguments.of(invalidCartItemRequest, true, false, false, 400, "购物车项99不存在！"),
                Arguments.of(wrongUserCartItemRequest, false, true, false, 400, "购物车项2不属于您！"),
                Arguments.of(outOfStockRequest, false, false, true, 400, "库存不足！")
        );
    }

    @ParameterizedTest
    @MethodSource("provideAddOrderTestCases")
    void testAddOrderDataFlow(
            OrderRequest request,
            boolean cartItemNotFound,
            boolean cartItemWrongUser,
            boolean outOfStock,
            int expectedCode,
            String expectedMessage) {

        User mockUser = new User();
        mockUser.setId(1);
        //when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

        if (cartItemNotFound) {
            when(cartItemDAO.getCartItemById(99)).thenReturn(null);
        } else {
            CartItem cartItem1 = new CartItem();
            cartItem1.setId(1);
            cartItem1.setQuantity(2);
            cartItem1.setUser(mockUser);
            Book book1 = new Book();
            book1.setId(1);
            cartItem1.setBook(book1);

            CartItem cartItem2 = new CartItem();
            cartItem2.setId(2);
            cartItem2.setQuantity(3);
            cartItem2.setUser(mockUser);
            Book book2 = new Book();
            book2.setId(2);
            cartItem2.setBook(book2);



            if (cartItemWrongUser) {
                User wrongUser = new User();
                wrongUser.setId(99);
                cartItem2.setUser(wrongUser);
                when(cartItemDAO.getCartItemById(2)).thenReturn(cartItem2);
            }
            else             if (outOfStock) {
                BookDetails bookDetails = new BookDetails();
                bookDetails.setStock(0); // 库存不足

                when(bookDAO.getBookDetailsById(anyInt())).thenReturn(bookDetails);
                when(cartItemDAO.getCartItemById(3)).thenReturn(cartItem1);
            } else {
                BookDetails bookDetails = new BookDetails();
                bookDetails.setStock(10); // 有库存
                when(cartItemDAO.getCartItemById(1)).thenReturn(cartItem1);
                when(cartItemDAO.getCartItemById(2)).thenReturn(cartItem2);
                when(bookDAO.getBookDetailsById(1)).thenReturn(bookDetails);
                when(bookDAO.getBookDetailsById(2)).thenReturn(bookDetails);
            }
        }

        Result<Order> result = orderService.addOrder(request);

        assertEquals(expectedCode, result.getCode());
        assertEquals(expectedMessage, result.getMessage());
    }

    // ========== deleteOrder 测试 ==========
    static Stream<Arguments> provideDeleteOrderTestCases() {
        return Stream.of(
                Arguments.of(1, true, true, 200, "正常删除"),
                Arguments.of(2, true, false, 403, "无权删除他人订单"),
                Arguments.of(3, false, false, 404, "订单不存在")
        );
    }
    @ParameterizedTest
    @MethodSource("provideDeleteOrderTestCases")
    void testDeleteOrderDataFlow(
            int orderId,
            boolean orderExists,
            boolean isUserOrder,
            int expectedCode,
            String expectedMessage) {
        when(orderDAO.existsById(orderId)).thenReturn(orderExists);

        if (orderExists) {
            Order mockOrder = new Order();
            mockOrder.setId(orderId);
            User mockUser = new User();
            mockUser.setId(1);
            mockOrder.setUser(mockUser);
            when(orderDAO.getOrderById(orderId)).thenReturn(mockOrder);
            when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

            if (!isUserOrder) {
                User wrongUser = new User();
                wrongUser.setId(99);
                mockOrder.setUser(wrongUser);
                when(orderDAO.getOrderById(orderId)).thenReturn(mockOrder);
            }
        } else {
            when(orderDAO.existsById(orderId)).thenReturn(false);
        }

        Result<Order> result = orderService.deleteOrder(orderId);

        assertEquals(expectedCode, result.getCode());

    }

    // ========== getAllOrders 测试 ==========
    @Test
    void testGetAllOrdersDataFlow() {
        Order order1 = new Order();
        order1.setId(1);
        Order order2 = new Order();
        order2.setId(2);
        List<Order> mockOrders = Arrays.asList(order1, order2);
        when(orderDAO.findAll()).thenReturn(mockOrders);

        Result<List<Order>> result = orderService.getAllOrders();

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(1, result.getData().get(0).getId());
        assertEquals(2, result.getData().get(1).getId());
    }

    // ========== searchAllOrders 测试 ==========
    @Test
    void testSearchAllOrdersDataFlow() {
        int page = 1;
        int pageSize = 10;
        LocalDateTime start = LocalDateTime.now().minusDays(1);
        LocalDateTime end = LocalDateTime.now();

        Order order1 = new Order();
        order1.setId(1);
        Order order2 = new Order();
        order2.setId(2);
        Page<Order> mockPage = new PageImpl<>(Arrays.asList(order1, order2));
        
        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, PageRequest.of(page - 1, pageSize)))
                .thenReturn(mockPage);

        Result<PageResponse<Order>> result = orderService.searchAllOrders("", page, pageSize, start, end);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().getTotal());
        assertEquals(1, result.getData().getContent().get(0).getId());
        assertEquals(2, result.getData().getContent().get(1).getId());
    }

    // ========== searchOrders 测试 ==========
    @Test
    void testSearchOrdersDataFlow() {

        User mockUser = new User();
        mockUser.setId(1);
        when(userDAO.getUserByUsername(anyString())).thenReturn(mockUser);

        int page = 1;
        int pageSize = 10;
        LocalDateTime start = LocalDateTime.now().minusDays(1);
        LocalDateTime end = LocalDateTime.now();

        Order order1 = new Order();
        order1.setId(1);
        Order order2 = new Order();
        order2.setId(2);
        Page<Order> mockPage = new PageImpl<>(Arrays.asList(order1, order2));
        
        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(
                start, end, 1, PageRequest.of(page - 1, pageSize)))
                .thenReturn(mockPage);

        Result<PageResponse<Order>> result = orderService.searchOrders("", start, end, page, pageSize);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().getTotal());
        assertEquals(1, result.getData().getContent().get(0).getId());
        assertEquals(2, result.getData().getContent().get(1).getId());
    }

    // ========== findCartItemById 测试 ==========
    @Test
    void testFindCartItemByIdDataFlow() {
        List<CartItem> cartItems = new ArrayList<>();
        CartItem item1 = new CartItem();
        item1.setId(1);
        item1.setQuantity(2);
        CartItem item2 = new CartItem();
        item2.setId(2);
        item2.setQuantity(1);
        cartItems.add(item1);
        cartItems.add(item2);

        CartItem result = orderService.findCartItemById(cartItems, 2);

        assertNotNull(result);
        assertEquals(2, result.getId());
        assertEquals(1, result.getQuantity());
    }
}