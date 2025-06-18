package org.example.service.Impl.LLM;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.*;
import java.util.stream.Stream;

import org.example.DAO.*;
import org.example.DTO.*;
import org.example.entity.*;
import org.example.repository.*;
import org.example.service.Impl.BookServiceImpl;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.data.domain.*;
import org.springframework.web.multipart.MultipartFile;

@ExtendWith(MockitoExtension.class)
class BookServiceImplTestLLM {

    @Mock private BookDAO bookDAO;
    @Mock private UploadRepository uploadRepository;
    @Mock private OrderDAO orderDAO;
    @Mock private CartItemDAO cartItemDAO;
    @Mock private OrderItemDAO orderItemDAO;
    @Mock private CommentDAO commentDAO;
    @Mock private BookTagRepository bookTagRepository;
    @Mock private TagRepository tagRepository;

    @InjectMocks private BookServiceImpl bookService;

    // ========== getBookById 测试 ==========
    static Stream<Arguments> provideGetBookByIdTestCases() {
        Book bookWithAll = new Book();
        bookWithAll.setId(1);
        bookWithAll.setBookDetails(new BookDetails());
        
        return Stream.of(
            Arguments.of(1, bookWithAll, 5, Arrays.asList("Tag1", "Tag2"), 200, "完整书籍信息"),
            Arguments.of(999, null, 0, Collections.emptyList(), 404, "书籍不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideGetBookByIdTestCases")
    void testGetBookByIdLLM(int id, Book mockBook, int commentCount, List<Tag> tags, int expectedStatus, String description) {
        when(bookDAO.getBookById(id)).thenReturn(mockBook);
        if (mockBook != null) {
            when(bookDAO.getBookDetailsById(id)).thenReturn(mockBook.getBookDetails());
            when(commentDAO.getCommentsByBookId(id)).thenReturn(Collections.nCopies(commentCount, null));
            when(bookTagRepository.findTidsByBid(id)).thenReturn(Collections.emptyList());
            when(tagRepository.findTagNamesByTids(anyList())).thenReturn(tags);
        }

        Result<Book> result = bookService.getBookById(id);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertNotNull(result.getData());
            assertEquals(commentCount, result.getData().getBookDetails().getComments().size());
            assertEquals(tags.size(), result.getData().getTags().size());
        }
    }

    // ========== updateBook 测试 ==========
    static Stream<Arguments> provideUpdateBookTestCases() {
        Book existingBook = new Book();
        existingBook.setId(1);
        existingBook.setTitle("Old Title");
        existingBook.setBookDetails(new BookDetails());
        
        Book updateData = new Book();
        updateData.setTitle("New Title");
        updateData.setBookDetails(new BookDetails());
        updateData.getBookDetails().setStock(20);
        
        return Stream.of(
            Arguments.of(1, existingBook, updateData, 200, "正常更新"),
            Arguments.of(999, null, updateData, 404, "书籍不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideUpdateBookTestCases")
    void testUpdateBookLLM(int id, Book existingBook, Book updateData, int expectedStatus, String description) {
        when(bookDAO.existsById(id)).thenReturn(existingBook != null);
        if (existingBook != null) {
            when(bookDAO.getBookById(id)).thenReturn(existingBook);
            when(bookDAO.save(any(Book.class))).thenReturn(existingBook);
        }

        Result<Book> result = bookService.updateBook(id, updateData);

        assertEquals(expectedStatus, result.getCode());
        if (expectedStatus == 200) {
            assertEquals("New Title", result.getData().getTitle());
            assertEquals(20, result.getData().getBookDetails().getStock());
        }
    }

    // ========== deleteBook 测试 ==========
    static Stream<Arguments> provideDeleteBookTestCases() {
        return Stream.of(
            Arguments.of(1, true, false, false, 200, "可删除书籍"),
            Arguments.of(2, true, true, false, 400, "书籍在购物车中"),
            Arguments.of(3, true, false, true, 400, "书籍已被购买"),
            Arguments.of(999, false, false, false, 404, "书籍不存在")
        );
    }

    @ParameterizedTest
    @MethodSource("provideDeleteBookTestCases")
    void testDeleteBookLLM(int id, boolean exists, boolean inCart, boolean inOrder, int expectedStatus, String description) {
        when(bookDAO.existsById(id)).thenReturn(exists);
        if (exists) {
            when(cartItemDAO.existsByBookId(id)).thenReturn(inCart);
            if (!inCart) {
                when(orderItemDAO.existsByBookId(id)).thenReturn(inOrder);
            }
        }

        Result<Book> result = bookService.deleteBook(id);

        assertEquals(expectedStatus, result.getCode());
    }

    // ========== addBook 测试 ==========
    @Test
    void testAddBookLLM() {
        Book newBook = new Book();
        newBook.setTitle("New Book");
        newBook.setBookDetails(new BookDetails());
        when(bookDAO.save(any(Book.class))).thenReturn(newBook);

        Result<Book> result = bookService.addBook(newBook);

        assertEquals(200, result.getCode());
        assertEquals(0, result.getData().getBookDetails().getRating());
    }

    // ========== getRecommendations 测试 ==========
    @Test
    void testGetRecommendationsLLM() {
        List<Book> mockBooks = new ArrayList<>();
        for (int i = 1; i <= 5; i++) {
            Book book = new Book();
            book.getBookDetails().setRating(i);
            mockBooks.add(book);
        }
        when(bookDAO.getBooksByRatingGreaterThanOrderByRatingDesc(0)).thenReturn(mockBooks);

        Result<List<Book>> result = bookService.getRecommendations(3);

        assertEquals(200, result.getCode());
        assertEquals(3, result.getData().size());
        assertEquals(1, result.getData().get(0).getBookDetails().getRating());
    }

    // ========== searchBooks 测试 ==========
    @Test
    void testSearchBooksLLM() {
        String keyword = "Java";
        int page = 1;
        int pageSize = 10;

        Page<Book> mockPage = new PageImpl<>(Arrays.asList(new Book(), new Book()));
        when(bookDAO.getBooksByTitleLikeOrAuthorLike(anyString(), anyString(), any(Pageable.class))).thenReturn(mockPage);
        when(commentDAO.countByBookId(anyInt())).thenReturn(5);

        Result<PageResponse<Book>> result = bookService.searchBooks(keyword, page, pageSize);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().getTotal());
    }

    // ========== searchBookByTagIds 测试 ==========
    @Test
    void testSearchBookByTagIdsLLM() {
        List<Long> tagIds = Arrays.asList(1L, 2L);
        int page = 1;
        int pageSize = 10;

        Page<Book> mockPage = new PageImpl<>(Collections.singletonList(new Book()));
        when(bookDAO.findBooksByTagIds(tagIds, PageRequest.of(page - 1, pageSize))).thenReturn(mockPage);
        when(commentDAO.countByBookId(anyInt())).thenReturn(3);

        Result<PageResponse<Book>> result = bookService.searchBookByTagIds(tagIds, page, pageSize);

        assertEquals(200, result.getCode());
        assertEquals(1, result.getData().getContent().size());
    }

    // ========== updateCover 测试 ==========
    static Stream<Arguments> provideUpdateCoverTestCases() {
        Book existingBook = new Book();
        existingBook.setId(1);
        
        return Stream.of(
            Arguments.of(1, existingBook, mock(MultipartFile.class), "http://example.com/cover.jpg", 200, "正常更新封面"),
            Arguments.of(999, null, mock(MultipartFile.class), null, 404, "书籍不存在"),
            Arguments.of(1, existingBook, null, null, 500, "文件为空")
        );
    }

    @ParameterizedTest
    @MethodSource("provideUpdateCoverTestCases")
    void testUpdateCoverLLM(int id, Book mockBook, MultipartFile file, String mockUrl, int expectedStatus, String description) throws IOException {
        when(bookDAO.getBookById(id)).thenReturn(mockBook);
        if (mockBook != null) {
            if (file == null) {
                when(uploadRepository.uploadFile(file, "image")).thenThrow(new IOException("文件为空"));
            } else {
                when(uploadRepository.uploadFile(file, "image")).thenReturn(mockUrl);
            }
        }

        Result<String> result = bookService.updateCover(id, file);

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

        Order order1 = new Order();
        OrderItem item1 = new OrderItem();
        Book book1 = new Book();
        book1.setId(1);
        book1.setTitle("Book 1");
        item1.setBook(book1);
        item1.setQuantity(5);

        Order order2 = new Order();
        OrderItem item2 = new OrderItem();
        Book book2 = new Book();
        book2.setId(2);
        book2.setTitle("Book 2");
        item2.setBook(book2);
        item2.setQuantity(3);

        order1.setOrderItems(Arrays.asList(item1, item2));
        order2.setOrderItems(Arrays.asList(item2, item1));

        when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end))
                .thenReturn(Arrays.asList(order1, order2));

        Result<List<SalesInfo>> result = bookService.rank(start, end, nums);

        assertEquals(200, result.getCode());
        assertEquals(2, result.getData().size());
        assertEquals(10, result.getData().get(0).getSales());
        assertEquals("Book 1", result.getData().get(0).getTitle());
    }
}