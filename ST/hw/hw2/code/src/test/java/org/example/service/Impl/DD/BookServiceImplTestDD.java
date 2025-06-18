package org.example.service.Impl.DD;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.mockito.Mockito.*;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;

import org.example.DAO.*;
import org.example.DTO.PageResponse;
import org.example.DTO.SalesInfo;
import org.example.entity.*;
import org.example.repository.BookTagRepository;
import org.example.repository.TagRepository;
import org.example.repository.UploadRepository;
import org.example.service.Impl.BookServiceImpl;
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
import org.springframework.web.multipart.MultipartFile;

@ExtendWith(MockitoExtension.class)
class BookServiceImplTestDD {

	@Mock
	private BookDAO bookDAO;
	@Mock
	private UploadRepository uploadRepository;
	@Mock
	private OrderDAO orderDAO;
	@Mock
	private CartItemDAO cartItemDAO;
	@Mock
	private OrderItemDAO orderItemDAO;
	@Mock
	private CommentDAO commentDAO;
	@Mock
	private BookTagRepository bookTagRepository;
	@Mock
	private TagRepository tagRepository;

	@InjectMocks
	private BookServiceImpl bookService;

	// ========== getBookById 测试 ==========
	static Stream<Arguments> provideGetBookByIdTestCases() {
		Book existingBook = new Book();
		existingBook.setId(1);
		existingBook.setBookDetails(new BookDetails());

		return Stream.of(Arguments.of(1, existingBook, 200, "书籍存在"), Arguments.of(999, null, 404, "书籍不存在"));
	}

	// ========== updateBook 测试 ==========
	static Stream<Arguments> provideUpdateBookTestCases() {
		Book existingBook = new Book();
		existingBook.setId(1);
		existingBook.setTitle("Old Title");
		existingBook.setBookDetails(new BookDetails());
		existingBook.getBookDetails().setStock(10);

		Book updateData = new Book();
		updateData.setTitle("New Title");
		updateData.setBookDetails(new BookDetails());
		updateData.getBookDetails().setStock(20);

		return Stream.of(Arguments.of(1, existingBook, updateData, 200, "正常更新"),
				Arguments.of(999, null, updateData, 404, "书籍不存在"));
	}

	// ========== deleteBook 测试 ==========
	static Stream<Arguments> provideDeleteBookTestCases() {
		return Stream.of(Arguments.of(1, true, false, false, 200, "可删除书籍"),
				Arguments.of(999, false, false, false, 404, "书籍不存在"),
				Arguments.of(2, true, true, false, 400, "书籍在购物车中"),
				Arguments.of(3, true, false, true, 400, "书籍已被购买"));
	}

	// ========== updateCover 测试 ==========
	static Stream<Arguments> provideUpdateCoverTestCases() {
		Book existingBook = new Book();
		existingBook.setId(1);

		MultipartFile mockFile = mock(MultipartFile.class);

		return Stream.of(Arguments.of(1, existingBook, mockFile, "http://example.com/cover.jpg", 200, "正常更新封面"),
				Arguments.of(999, null, mockFile, null, 404, "书籍不存在"));
	}

	@ParameterizedTest
	@MethodSource("provideGetBookByIdTestCases")
	void testGetBookById(int id, Book mockBook, int expectedStatus, String description) {
		when(bookDAO.getBookById(id)).thenReturn(mockBook);
		when(bookDAO.getBookDetailsById(id)).thenReturn(mockBook != null ? mockBook.getBookDetails() : null);
		bookDAO.getBookDetailsById(id);

		Result<Book> result = bookService.getBookById(id);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertNotNull(result.getData());
		}
	}

	// ========== getRecommendations 测试 ==========
	@Test
	void testGetRecommendations() {
		List<Book> mockBooks = new ArrayList<>();
		for (int i = 0; i < 5; i++) {
			Book book = new Book();
			book.getBookDetails().setRating(i + 1);
			mockBooks.add(book);
		}
		when(bookDAO.getBooksByRatingGreaterThanOrderByRatingDesc(0)).thenReturn(mockBooks);

		Result<List<Book>> result = bookService.getRecommendations(3);

		assertEquals(200, result.getCode());
		assertEquals(3, result.getData().size());
		assertEquals(1, result.getData().get(0).getBookDetails().getRating());
	}

	// ========== addBook 测试 ==========
	@Test
	void testAddBook() {
		Book newBook = new Book();
		newBook.setTitle("New Book");
		newBook.setBookDetails(new BookDetails());
		when(bookDAO.save(any(Book.class))).thenReturn(newBook);

		Result<Book> result = bookService.addBook(newBook);

		assertEquals(200, result.getCode());
		assertEquals(0, result.getData().getBookDetails().getRating());
		assertEquals(0, result.getData().getBookDetails().getSales());
	}

	@ParameterizedTest
	@MethodSource("provideUpdateBookTestCases")
	void testUpdateBook(int id, Book mockBook, Book updateData, int expectedStatus, String description) {
		when(bookDAO.existsById(id)).thenReturn(mockBook != null);
		if (mockBook != null) {
			when(bookDAO.getBookById(id)).thenReturn(mockBook);
			when(bookDAO.save(any(Book.class))).thenReturn(mockBook);
		}

		Result<Book> result = bookService.updateBook(id, updateData);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertEquals("New Title", result.getData().getTitle());
			assertEquals(20, result.getData().getBookDetails().getStock());
		}
	}

	@ParameterizedTest
	@MethodSource("provideDeleteBookTestCases")
	void testDeleteBook(int id, boolean exists, boolean inCart, boolean inOrder, int expectedStatus,
			String description) {
		when(bookDAO.existsById(id)).thenReturn(exists);
		if (exists) {
			when(cartItemDAO.existsByBookId(id)).thenReturn(inCart);
			when(orderItemDAO.existsByBookId(id)).thenReturn(inOrder);
		}
		orderItemDAO.existsByBookId(id);

		Result<Book> result = bookService.deleteBook(id);

		assertEquals(expectedStatus, result.getCode());
	}

	// ========== searchBooks 测试 ==========
	@Test
	void testSearchBooks() {
		String keyword = "Java";
		int page = 1;
		int pageSize = 10;

		List<Book> mockBooks = Arrays.asList(new Book(), new Book());
		Page<Book> mockPage = new PageImpl<>(mockBooks, PageRequest.of(page - 1, pageSize), mockBooks.size());

		when(bookDAO.getBooksByTitleLikeOrAuthorLike(anyString(), anyString(), any(Pageable.class)))
				.thenReturn(mockPage);
		when(commentDAO.countByBookId(anyInt())).thenReturn(5);

		Result<PageResponse<Book>> result = bookService.searchBooks(keyword, page, pageSize);

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().getTotal());
		assertEquals(5, result.getData().getContent().get(0).getBookDetails().getComments().size());
	}

	// ========== searchBookByTagIds 测试 ==========
	@Test
	void testSearchBookByTagIds() {
		List<Long> tagIds = Arrays.asList(1L, 2L);
		int page = 1;
		int pageSize = 10;

		List<Book> mockBooks = Arrays.asList(new Book());
		Page<Book> mockPage = new PageImpl<>(mockBooks, PageRequest.of(page - 1, pageSize), mockBooks.size());

		when(bookDAO.findBooksByTagIds(tagIds, PageRequest.of(page - 1, pageSize))).thenReturn(mockPage);
		when(commentDAO.countByBookId(anyInt())).thenReturn(3);

		Result<PageResponse<Book>> result = bookService.searchBookByTagIds(tagIds, page, pageSize);

		assertEquals(200, result.getCode());
		assertEquals(1, result.getData().getContent().size());
	}

	@ParameterizedTest
	@MethodSource("provideUpdateCoverTestCases")
	void testUpdateCover(int id, Book mockBook, MultipartFile file, String mockUrl, int expectedStatus,
			String description) throws IOException {
		when(bookDAO.getBookById(id)).thenReturn(mockBook);
		if (mockBook != null) {
			when(uploadRepository.uploadFile(file, "image")).thenReturn(mockUrl);
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

		order1.setOrderItems(Arrays.asList(item1));
		order2.setOrderItems(Arrays.asList(item2));

		when(orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end))
				.thenReturn(Arrays.asList(order1, order2));

		Result<List<SalesInfo>> result = bookService.rank(start, end, nums);

		assertEquals(200, result.getCode());
		assertEquals(2, result.getData().size());
		assertEquals(5, result.getData().get(0).getSales());
	}
}