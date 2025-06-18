package org.example.service.Impl.DataFlow;

import org.example.DAO.OrderDAO;
import org.example.DAO.UserDAO;
import org.example.DTO.*;
import org.example.entity.*;
import org.example.repository.UploadRepository;
import org.example.service.Impl.MyUserDetailsService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.*;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
class MyUserDetailsServiceTestDF {

    @Mock
    private UserDAO userDAO;
    @Mock
    private UploadRepository uploadRepository;
    @Mock
    private OrderDAO orderDAO;
    
    @InjectMocks
    private MyUserDetailsService userDetailsService;

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

    // ========== loadUserByUsername 测试 ==========
    static Stream<Arguments> provideLoadUserByUsernameTestCases() {
        User userWithUserRole = new User();
        userWithUserRole.setUsername("user1");
        userWithUserRole.setRole("user");
        userWithUserRole.setEnabled(true);

        User userWithAdminRole = new User();
        userWithAdminRole.setUsername("admin1");
        userWithAdminRole.setRole("admin");
        userWithAdminRole.setEnabled(true);

        UserAuth userAuth = new UserAuth();
        userAuth.setUsername("user1");
        userAuth.setPassword("encodedPassword");

        return Stream.of(
                Arguments.of("user1", userWithUserRole, userAuth, "ROLE_user", "验证用户角色"),
                Arguments.of("admin1", userWithAdminRole, userAuth, "ROLE_admin", "验证管理员角色"),
                Arguments.of("nonExistentUser", null, null, null, "验证不存在的用户")
        );
    }

    @ParameterizedTest
    @MethodSource("provideLoadUserByUsernameTestCases")
    void testLoadUserByUsername(String username, User mockUser, UserAuth mockUserAuth, String expectedRole, String description) {
        if (mockUser != null) {
            when(userDAO.getUserByUsername(username)).thenReturn(mockUser);
            when(userDAO.findUserAuthByUsername(username)).thenReturn(mockUserAuth);
        } else {
            when(userDAO.getUserByUsername(username)).thenReturn(null);
        }

        if(mockUser==null) {
            assertThrows(UsernameNotFoundException.class, () -> {
                userDetailsService.loadUserByUsername(username);
            });
        }
else
		{
			UserDetails userDetails = userDetailsService.loadUserByUsername(username);

			assertNotNull(userDetails);
			assertEquals(username, userDetails.getUsername());
			assertTrue(userDetails.getAuthorities().stream().anyMatch(a -> a.getAuthority().equals(expectedRole)));
		}
    }

    // ========== updateUser 测试 ==========
    static Stream<Arguments> provideUpdateUserTestCases() {
        User currentUser = new User();
        currentUser.setUsername("oldUser");
        currentUser.setEmail("old@example.com");

        UserProfile request1 = new UserProfile();
        request1.setUsername("newUser");
        request1.setEmail("new@example.com");

        UserProfile request2 = new UserProfile();
        request2.setUsername("existingUser");
        request2.setEmail("new@example.com");

        return Stream.of(
                Arguments.of(currentUser, request1, false, 200, "正常更新"),
                Arguments.of(currentUser, request2, true, 400, "用户名已存在")
        );
    }
    @ParameterizedTest
    @MethodSource("provideUpdateUserTestCases")
    void testUpdateUserDataFlow(User currentUser, UserProfile request, boolean userExists, int expectedCode, String description) {
        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
                when(userDAO.existsUserByUsername(request.getUsername())).thenReturn(userExists);
        if(!userExists) {
            when(userDAO.save(any(User.class))).thenReturn(currentUser);
        }

        Result<User> result = userDetailsService.updateUser(request);

        assertEquals(expectedCode, result.getCode(), description);
        if (expectedCode == 200) {
            assertEquals(request.getUsername(), result.getData().getUsername());
            assertEquals(request.getEmail(), result.getData().getEmail());
        } else {
            assertNull(result.getData());
        }
    }

    // ========== addUser 测试 ==========
    static Stream<Arguments> provideAddUserTestCases() {
        RegisterRequest validRequest = new RegisterRequest();
        validRequest.setUsername("newUser");
        validRequest.setEmail("new@example.com");
        validRequest.setPassword("password");

        RegisterRequest existingUsername = new RegisterRequest();
        existingUsername.setUsername("existingUser");
        existingUsername.setEmail("new@example.com");
        existingUsername.setPassword("password");

        RegisterRequest existingEmail = new RegisterRequest();
        existingEmail.setUsername("newUser");
        existingEmail.setEmail("existing@example.com");
        existingEmail.setPassword("password");

        return Stream.of(
                Arguments.of(validRequest, false, false, 200, "正常注册"),
                Arguments.of(existingUsername, true, false, 400, "用户名已存在"),
                Arguments.of(existingEmail, false, true, 400, "邮箱已存在")
        );
    }
    @ParameterizedTest
    @MethodSource("provideAddUserTestCases")
    void testAddUserDataFlow( RegisterRequest request, boolean usernameExists, boolean emailExists, int expectedCode, String description) {
        when(userDAO.existsUserByUsername(request.getUsername())).thenReturn(usernameExists);
        if (!usernameExists) {
            when(userDAO.existsUserByEmail(request.getEmail())).thenReturn(emailExists);
        }
        if (!usernameExists && !emailExists) {
            when(userDAO.save(any(User.class), any(UserAuth.class))).thenReturn(new User());
        }

        User newUser = new User();
        newUser.setUsername(request.getUsername());
        newUser.setEmail(request.getEmail());
        newUser.setRole("user");
        newUser.setEnabled(true);
        newUser.setLevel(1);
        newUser.setAvatar("default_avatar");
        newUser.setTel("");
        newUser.setAboutMe("");

        UserAuth userAuth = new UserAuth();
        userAuth.setUsername(request.getUsername());
        userAuth.setPassword(new BCryptPasswordEncoder().encode(request.getPassword()));


        Result<User> result = userDetailsService.addUser(request);

        assertEquals(expectedCode, result.getCode(), description);
    }

    // ========== deleteUser 测试 ==========
    @Test
    void testDeleteUserDataFlow() {
        User userToDelete = new User();
        userToDelete.setId(1);

        when(userDAO.getUserById(1)).thenReturn(userToDelete);
        doNothing().when(userDAO).deleteUser(userToDelete);

        Result<User> result = userDetailsService.deleteUser(1);

        assertEquals(200, result.getCode());
        verify(userDAO).deleteUser(userToDelete);
    }

    // ========== disableUser 测试 ==========
    static Stream<Arguments> provideDisableUserTestCases() {
        User normalUser = new User();
        normalUser.setId(1);
        normalUser.setRole("user");
        normalUser.setEnabled(true);

        User adminUser = new User();
        adminUser.setId(2);
        adminUser.setRole("admin");
        adminUser.setEnabled(true);

        return Stream.of(
                Arguments.of(1, normalUser, 200, "正常用户"),
                Arguments.of(2, adminUser, 403, "管理员用户"),
                Arguments.of(3, null, 404, "用户不存在")
        );
    }
    @ParameterizedTest
    @MethodSource("provideDisableUserTestCases")
    void testDisableUserDataFlow( int userId, User user, int expectedCode, String description) {
        when(userDAO.getUserById(userId)).thenReturn(user);
        if (user != null && expectedCode == 200) {
            when(userDAO.save(user)).thenReturn(user);
        }

        Result<User> result = userDetailsService.disableUser(userId);

        assertEquals(expectedCode, result.getCode(), description);
        if (expectedCode == 200) {
            assertFalse(result.getData().isEnabled());
        } else if (expectedCode == 403) {
            assertTrue(result.getMessage().contains("无法禁用管理员！"));
        } else {
            assertNull(result.getData());
        }
    }

    // ========== enableUser 测试 ==========
    @Test
    void testEnableUserDataFlow() {
        User userToEnable = new User();
        userToEnable.setId(1);
        userToEnable.setEnabled(false);

        when(userDAO.getUserById(1)).thenReturn(userToEnable);
        when(userDAO.save(userToEnable)).thenReturn(userToEnable);

        Result<User> result = userDetailsService.enableUser(1);

        assertEquals(200, result.getCode());
        assertTrue(result.getData().isEnabled());
    }

    // ========== updateAvatar 测试 ==========
    @Test
    void testUpdateAvatarDataFlow() throws IOException {
        User currentUser = new User();
        currentUser.setUsername("user1");
        currentUser.setAvatar("old_avatar");

        MultipartFile file = mock(MultipartFile.class);
        String newAvatarUrl = "http://example.com/new_avatar.jpg";

        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(uploadRepository.uploadFile(file, "image")).thenReturn(newAvatarUrl);
        when(userDAO.save(currentUser)).thenReturn(currentUser);

        Result<String> result = userDetailsService.updateAvatar(file);

        assertEquals(200, result.getCode());
        assertEquals(newAvatarUrl, result.getData());
        assertEquals(newAvatarUrl, currentUser.getAvatar());
    }

    // ========== rank 测试 ==========
    @Test
    void testRankDataFlow() {
        LocalDateTime start = LocalDateTime.now().minusDays(7);
        LocalDateTime end = LocalDateTime.now();
        int nums = 2;

        User user1 = new User();
        user1.setId(1);
        user1.setUsername("user1");

        User user2 = new User();
        user2.setId(2);
        user2.setUsername("user2");

        Book book1 = new Book();
        book1.setId(1);
        book1.setPrice(100);

        Book book2 = new Book();
        book2.setId(2);
        book2.setPrice(200);

        Order order1 = new Order();
        order1.setUser(user1);
        OrderItem item1 = new OrderItem();
        item1.setBook(book1);
        item1.setQuantity(2);
        order1.setOrderItems(List.of(item1));

        Order order2 = new Order();
        order2.setUser(user2);
        OrderItem item2 = new OrderItem();
        item2.setBook(book2);
        item2.setQuantity(3);
        order2.setOrderItems(List.of(item2));

        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end))
                .thenReturn(List.of(order1, order2));

        Result<List<SpentInfo>> result = userDetailsService.rank(start, end, nums);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        
        // 验证数据转换是否正确
        SpentInfo first = result.getData().get(0);
        assertEquals(2, first.getId());
        assertEquals("user2", first.getUsername());
        assertEquals(600, first.getTotalSpent());

        SpentInfo second = result.getData().get(1);
        assertEquals(1, second.getId());
        assertEquals("user1", second.getUsername());
        assertEquals(200, second.getTotalSpent());
    }

    // ========== statistics 测试 ==========
    @Test
    void testStatisticsDataFlow() {
        LocalDateTime start = LocalDateTime.now().minusDays(7);
        LocalDateTime end = LocalDateTime.now();

        User currentUser = new User();
        currentUser.setId(1);

        Book book1 = new Book();
        book1.setId(1);
        book1.setTitle("Book 1");
        book1.setPrice(100);

        Book book2 = new Book();
        book2.setId(2);
        book2.setTitle("Book 2");
        book2.setPrice(200);

        Order order1 = new Order();
        order1.setUser(currentUser);
        OrderItem item1 = new OrderItem();
        item1.setBook(book1);
        item1.setQuantity(2);
        OrderItem item2 = new OrderItem();
        item2.setBook(book1);
        item2.setQuantity(1);
        OrderItem item3 = new OrderItem();
        item3.setBook(book2);
        item3.setQuantity(1);
        order1.setOrderItems(List.of(item1, item2, item3));

        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, currentUser.getId()))
                .thenReturn(List.of(order1));

        Result<List<PurchaseInfo>> result = userDetailsService.statistics(start, end);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        
        // 验证数据聚合是否正确
        PurchaseInfo book1Info = result.getData().get(0);
        assertEquals(1, book1Info.getId());
        assertEquals("Book 1", book1Info.getTitle());
        assertEquals(3, book1Info.getCount()); // 2 + 1
        assertEquals(100, book1Info.getPrice());

        PurchaseInfo book2Info = result.getData().get(1);
        assertEquals(2, book2Info.getId());
        assertEquals("Book 2", book2Info.getTitle());
        assertEquals(1, book2Info.getCount());
        assertEquals(200, book2Info.getPrice());
    }

    // ========== getUserById 测试 ==========
    @Test
    void testGetUserByIdDataFlow() {
        User expectedUser = new User();
        expectedUser.setId(1);
        expectedUser.setUsername("user1");
        expectedUser.setEmail("user1@example.com");

        when(userDAO.getUserById(1)).thenReturn(expectedUser);

        Result<User> result = userDetailsService.getUserById(1);

        assertEquals(200, result.getCode());
        assertSame(expectedUser, result.getData());
        assertEquals("user1", result.getData().getUsername());
        assertEquals("user1@example.com", result.getData().getEmail());
    }

    @Test
    void testGetUserByIdNotFoundDataFlow() {
        when(userDAO.getUserById(1)).thenReturn(null);

        Result<User> result = userDetailsService.getUserById(1);

        assertEquals(404, result.getCode());
        assertEquals("用户不存在！", result.getMessage());
        assertNull(result.getData());
    }

    // ========== getUserByUsername 测试 ==========
    @Test
    void testGetUserByUsernameDataFlow() {
        User expectedUser = new User();
        expectedUser.setUsername("user1");
        expectedUser.setRole("admin");
        expectedUser.setEnabled(true);

        when(userDAO.getUserByUsername("user1")).thenReturn(expectedUser);

        Result<User> result = userDetailsService.getUserByUsername("user1");

        assertEquals(200, result.getCode());
        assertSame(expectedUser, result.getData());
        assertEquals("admin", result.getData().getRole());
        assertTrue(result.getData().isEnabled());
    }

    @Test
    void testGetUserByUsernameNotFoundDataFlow() {
        when(userDAO.getUserByUsername("unknown")).thenReturn(null);

        Result<User> result = userDetailsService.getUserByUsername("unknown");

        assertEquals(404, result.getCode());
        assertEquals("用户不存在！", result.getMessage());
        assertNull(result.getData());
    }

    // ========== getAllUsers 测试 ==========
    @Test
    void testGetAllUsersDataFlow() {
        List<User> expectedUsers = Arrays.asList(
                createUser(1, "user1", "user"),
                createUser(2, "user2", "user"),
                createUser(3, "admin1", "admin")
        );
        when(userDAO.findAll()).thenReturn(expectedUsers);

        Result<List<User>> result = userDetailsService.getAllUsers();

        assertEquals(200, result.getCode());
        assertEquals(3, result.getData().size());

        // 验证数据完整性
        assertEquals("user1", result.getData().get(0).getUsername());
        assertEquals("user2", result.getData().get(1).getUsername());
        assertEquals("admin1", result.getData().get(2).getUsername());
        assertEquals("admin", result.getData().get(2).getRole());
    }

    private User createUser(int id, String username, String role) {
        User user = new User();
        user.setId(id);
        user.setUsername(username);
        user.setRole(role);
        return user;
    }
}