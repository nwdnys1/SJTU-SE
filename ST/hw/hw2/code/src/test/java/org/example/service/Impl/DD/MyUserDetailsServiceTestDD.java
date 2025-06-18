package org.example.service.Impl.DD;

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
import org.springframework.security.authentication.DisabledException;
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
class MyUserDetailsServiceTestDD {

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
        User existingUser = new User();
        existingUser.setUsername("user1");
        existingUser.setEnabled(true);
        existingUser.setRole("user");

        UserAuth userAuth = new UserAuth();
        userAuth.setUsername("user1");
        userAuth.setPassword("encodedPassword");

        return Stream.of(
                Arguments.of("user1", existingUser, userAuth, false, "正常用户"),
                Arguments.of("user2", null, null, true, "用户不存在"),
                Arguments.of("user1", existingUser, userAuth, false, "禁用用户")
        );
    }

    @ParameterizedTest
    @MethodSource("provideLoadUserByUsernameTestCases")
    void testLoadUserByUsername(String username, User mockUser, UserAuth mockUserAuth, boolean shouldThrow, String description) {
        if (mockUser != null) {
            when(userDAO.getUserByUsername(username)).thenReturn(mockUser);
            when(userDAO.findUserAuthByUsername(username)).thenReturn(mockUserAuth);
        } else {
            when(userDAO.getUserByUsername(username)).thenReturn(null);
        }

        if (shouldThrow) {
            assertThrows(UsernameNotFoundException.class, () -> {
                userDetailsService.loadUserByUsername(username);
            });
        } else {
            UserDetails userDetails = userDetailsService.loadUserByUsername(username);
            assertNotNull(userDetails);
            assertEquals(username, userDetails.getUsername());
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
    void testUpdateUser(User currentUser, UserProfile request, boolean usernameExists, int expectedStatus, String description) {

        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(userDAO.existsUserByUsername(request.getUsername())).thenReturn(usernameExists);
		if(!usernameExists) {
			when(userDAO.save(any(User.class))).thenReturn(currentUser);
		}


        Result<User> result = userDetailsService.updateUser(request);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertEquals(request.getUsername(), result.getData().getUsername());
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
    void testAddUser(RegisterRequest request, boolean usernameExists, boolean emailExists, int expectedStatus, String description) {
        when(userDAO.existsUserByUsername(request.getUsername())).thenReturn(usernameExists);
        if (!usernameExists) {
            when(userDAO.existsUserByEmail(request.getEmail())).thenReturn(emailExists);
        }
        if (!usernameExists && !emailExists) {
            when(userDAO.save(any(User.class), any(UserAuth.class))).thenReturn(new User());
        }

        Result<User> result = userDetailsService.addUser(request);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertNotNull(result.getData());
        }
    }

    // ========== deleteUser 测试 ==========
    static Stream<Arguments> provideDeleteUserTestCases() {
        User existingUser = new User();
        existingUser.setId(1);

        return Stream.of(
                Arguments.of(1, existingUser, 200, "用户存在"),
                Arguments.of(2, null, 404, "用户不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideDeleteUserTestCases")
    void testDeleteUser(int id, User mockUser, int expectedStatus, String description) {
        when(userDAO.getUserById(id)).thenReturn(mockUser);

        Result<User> result = userDetailsService.deleteUser(id);

        assertEquals(expectedStatus, result.getCode());
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
    void testDisableUser(int id, User mockUser, int expectedStatus, String description) {
        when(userDAO.getUserById(id)).thenReturn(mockUser);
		if (mockUser != null && expectedStatus == 200) {
			when(userDAO.save(mockUser)).thenReturn(mockUser);
		}

		// 模拟禁用用户
		Result<User> result = userDetailsService.disableUser(id);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertFalse(result.getData().isEnabled());
		}
		if (expectedStatus == 403) {
			assertEquals("无法禁用管理员！", result.getMessage());
		}
		if (expectedStatus == 404) {
			assertEquals("用户不存在！", result.getMessage());
		}


    }

    // ========== enableUser 测试 ==========
    @Test
    void testEnableUser() {
        User disabledUser = new User();
        disabledUser.setId(1);
        disabledUser.setEnabled(false);

        when(userDAO.getUserById(1)).thenReturn(disabledUser);
		when(userDAO.save(disabledUser)).thenReturn(disabledUser);

        Result<User> result = userDetailsService.enableUser(1);

        assertEquals(200, result.getCode());
        assertTrue(result.getData().isEnabled());
    }

    // ========== updateAvatar 测试 ==========
    static Stream<Arguments> provideUpdateAvatarTestCases() {
        MultipartFile validFile = mock(MultipartFile.class);
        MultipartFile invalidFile = mock(MultipartFile.class);

        return Stream.of(
                Arguments.of(validFile, "http://example.com/avatar.jpg", false, 200, "正常上传"),
                Arguments.of(invalidFile, null, true, 500, "上传失败")
        );
    }

    @ParameterizedTest
    @MethodSource("provideUpdateAvatarTestCases")
    void testUpdateAvatar(MultipartFile file, String mockUrl, boolean shouldThrow, int expectedStatus, String description) throws IOException {
        User currentUser = new User();
        currentUser.setUsername("user1");

        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(userDetailsService.getUser()).thenReturn(currentUser);

        if (shouldThrow) {
            when(uploadRepository.uploadFile(file, "image")).thenThrow(new IOException());
        } else {
            when(uploadRepository.uploadFile(file, "image")).thenReturn(mockUrl);
        }

        Result<String> result = userDetailsService.updateAvatar(file);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertEquals(mockUrl, result.getData());
        }
    }

    // ========== rank 测试 ==========
    @Test
    void testRank() {
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
        order1.setOrderItems(List.of(item1, item1));

        Order order2 = new Order();
        order2.setUser(user2);
        OrderItem item2 = new OrderItem();
        item2.setBook(book2);
        item2.setQuantity(3);
        order2.setOrderItems(List.of(item2, item2));

        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end))
                .thenReturn(List.of(order1, order2));

        Result<List<SpentInfo>> result = userDetailsService.rank(start, end, nums);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(1200, result.getData().get(0).getTotalSpent()); // user2 spent 600
        assertEquals(400, result.getData().get(1).getTotalSpent()); // user1 spent 200
    }

    // ========== statistics 测试 ==========
    @Test
    void testStatistics() {
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
        item2.setBook(book2);
        item2.setQuantity(1);
        order1.setOrderItems(List.of(item1, item2));

        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, currentUser.getId()))
                .thenReturn(List.of(order1));
        when(userDetailsService.getUser()).thenReturn(currentUser);

        Result<List<PurchaseInfo>> result = userDetailsService.statistics(start, end);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(2, result.getData().get(0).getCount()); // Book 1 purchased 2 times
        assertEquals(1, result.getData().get(1).getCount()); // Book 2 purchased 1 time
    }

    // ========== getUserById 测试 ==========
    static Stream<Arguments> provideGetUserByIdTestCases() {
        User existingUser = new User();
        existingUser.setId(1);
        existingUser.setUsername("user1");

        return Stream.of(
                Arguments.of(1, existingUser, 200, "用户存在"),
                Arguments.of(2, null, 404, "用户不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideGetUserByIdTestCases")
    void testGetUserById(int uid, User mockUser, int expectedStatus, String description) {
        when(userDAO.getUserById(uid)).thenReturn(mockUser);

        Result<User> result = userDetailsService.getUserById(uid);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertEquals(mockUser, result.getData());
        } else {
            assertEquals("用户不存在！", result.getMessage());
        }
    }

    // ========== getUserByUsername 测试 ==========
    static Stream<Arguments> provideGetUserByUsernameTestCases() {
        User existingUser = new User();
        existingUser.setUsername("user1");

        return Stream.of(
                Arguments.of("user1", existingUser, 200, "用户存在"),
                Arguments.of("nonexistent", null, 404, "用户不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideGetUserByUsernameTestCases")
    void testGetUserByUsername(String username, User mockUser, int expectedStatus, String description) {
        when(userDAO.getUserByUsername(username)).thenReturn(mockUser);

        Result<User> result = userDetailsService.getUserByUsername(username);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertEquals(mockUser, result.getData());
        } else {
            assertEquals("用户不存在！", result.getMessage());
        }
    }

    // ========== getAllUsers 测试 ==========
    @Test
    void testGetAllUsers() {
        List<User> mockUsers = Arrays.asList(
                new User(),
                new User(),
                new User()
        );
        when(userDAO.findAll()).thenReturn(mockUsers);

        Result<List<User>> result = userDetailsService.getAllUsers();

        assertEquals(200, result.getCode());
        assertEquals(3, result.getData().size());
    }
}