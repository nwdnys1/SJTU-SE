package org.example.service.Impl.LLM;

import org.example.DAO.*;
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
class MyUserDetailsServiceTestLLM {

    @Mock private UserDAO userDAO;
    @Mock private UploadRepository uploadRepository;
    @Mock private OrderDAO orderDAO;
    
    @InjectMocks private MyUserDetailsService userDetailsService;

    @BeforeEach
    public void setup() {
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

        UserAuth userAuth = new UserAuth();
        userAuth.setUsername("user1");
        userAuth.setPassword("encodedPassword");

        return Stream.of(
            Arguments.of("user1", userWithUserRole, userAuth, "ROLE_user", false, "正常用户"),
            Arguments.of("admin1", null, null, null, true, "用户不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideLoadUserByUsernameTestCases")
    void testLoadUserByUsernameLLM(String username, User mockUser, UserAuth mockUserAuth, 
                                 String expectedRole, boolean shouldThrow, String description) {
        when(userDAO.getUserByUsername(username)).thenReturn(mockUser);
        if (mockUser != null) {
            when(userDAO.findUserAuthByUsername(username)).thenReturn(mockUserAuth);
        }

        if (shouldThrow) {
            assertThrows(UsernameNotFoundException.class, () -> {
                userDetailsService.loadUserByUsername(username);
            });
        } else {
            UserDetails userDetails = userDetailsService.loadUserByUsername(username);
            assertNotNull(userDetails);
            assertEquals(username, userDetails.getUsername());
            assertTrue(userDetails.getAuthorities().stream()
                    .anyMatch(a -> a.getAuthority().equals(expectedRole)));
        }
    }

    // ========== updateUser 测试 ==========
    static Stream<Arguments> provideUpdateUserTestCases() {
        User currentUser = new User();
        currentUser.setUsername("oldUser");
        currentUser.setEmail("old@example.com");

        UserProfile validRequest = new UserProfile();
        validRequest.setUsername("newUser");
        validRequest.setEmail("new@example.com");

        return Stream.of(
                Arguments.of(currentUser, validRequest, true, 400, "用户名已存在"),
            Arguments.of(currentUser, validRequest, false, 200, "正常更新")

        );
    }

    @ParameterizedTest
    @MethodSource("provideUpdateUserTestCases")
    void testUpdateUserLLM(User currentUser, UserProfile request, 
                         boolean usernameExists, int expectedStatus, String description) {
        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(userDAO.existsUserByUsername(request.getUsername())).thenReturn(usernameExists);
        if (!usernameExists) {
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

        return Stream.of(
            Arguments.of(validRequest, false, false, 200, "正常注册"),
            Arguments.of(validRequest, true, false, 400, "用户名已存在"),
            Arguments.of(validRequest, false, true, 400, "邮箱已存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideAddUserTestCases")
    void testAddUserLLM(RegisterRequest request, boolean usernameExists, 
                       boolean emailExists, int expectedStatus, String description) {
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
    void testDisableUserLLM(int userId, User user, int expectedStatus, String description) {
        when(userDAO.getUserById(userId)).thenReturn(user);
        if (user != null && expectedStatus == 200) {
            when(userDAO.save(user)).thenReturn(user);
        }

        Result<User> result = userDetailsService.disableUser(userId);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertFalse(result.getData().isEnabled());
        }
    }

    // ========== enableUser 测试 ==========
    static Stream<Arguments> provideEnableUserTestCases() {
        User disabledUser = new User();
        disabledUser.setId(1);
        disabledUser.setEnabled(false);

        return Stream.of(
            Arguments.of(1, disabledUser, 200, "正常用户"),
            Arguments.of(2, null, 404, "用户不存在")
        );
    }
    @ParameterizedTest
    @MethodSource("provideEnableUserTestCases")
    void testEnableUserLLM(int userId, User user, int expectedStatus, String description) {
        when(userDAO.getUserById(userId)).thenReturn(user);
        if (user != null && expectedStatus == 200) {
            when(userDAO.save(user)).thenReturn(user);
        }

        Result<User> result = userDetailsService.enableUser(userId);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertTrue(result.getData().isEnabled());
        }
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
    void testRankLLM() {
        LocalDateTime start = LocalDateTime.now().minusDays(7);
        LocalDateTime end = LocalDateTime.now();
        int nums = 2;

        User user1 = new User();
        user1.setId(1);
        Order order1 = new Order();
        order1.setUser(user1);
        OrderItem item1 = new OrderItem();
        item1.setBook(new Book());
        item1.getBook().setPrice(100);
        item1.setQuantity(2);
        order1.setOrderItems(List.of(item1));

        Order order2 = new Order();
        order2.setUser(user1);
        OrderItem item2 = new OrderItem();
        item2.setBook(new Book());
        item2.getBook().setPrice(50);
        item2.setQuantity(1);
        order2.setOrderItems(List.of(item2));

        Order order3 = new Order();
        order3.setUser(new User());
        order3.setOrderItems(List.of(item1, item2));



        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end))
                .thenReturn(List.of(order1,order2,order3));

        Result<List<SpentInfo>> result = userDetailsService.rank(start, end, nums);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(250, result.getData().get(0).getTotalSpent());
    }

    // ========== statistics 测试 ==========
    @Test
    void testStatisticsLLM() {
        LocalDateTime start = LocalDateTime.now().minusDays(7);
        LocalDateTime end = LocalDateTime.now();

        User currentUser = new User();
        currentUser.setId(1);
        Order order1 = new Order();
        order1.setUser(currentUser);
        OrderItem item1 = new OrderItem();
        item1.setBook(new Book());
        item1.getBook().setId(1);
        item1.getBook().setTitle("Book 1");
        item1.setQuantity(2);
        order1.setOrderItems(List.of(item1));

        Order order2 = new Order();
        order2.setUser(currentUser);
        OrderItem item2 = new OrderItem();
        item2.setBook(new Book());
        item2.getBook().setId(1);
        item2.getBook().setTitle("Book 1");
        item2.getBook().setPrice(50);
        item2.setQuantity(1);
        OrderItem item3 = new OrderItem();
        item3.setBook(new Book());
        item3.getBook().setId(2);
        item3.getBook().setTitle("Book 2");
        item3.setQuantity(1);
        order2.setOrderItems(List.of(item2,item3));



        when(userDAO.getUserByUsername(anyString())).thenReturn(currentUser);
        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(
                start, end, currentUser.getId())).thenReturn(List.of(order1,order2));

        Result<List<PurchaseInfo>> result = userDetailsService.statistics(start, end);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(3, result.getData().get(0).getCount());
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
    void testGetAllUsersLLM() {
        when(userDAO.findAll()).thenReturn(Arrays.asList(new User(), new User()));

        Result<List<User>> result = userDetailsService.getAllUsers();

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
    }
}