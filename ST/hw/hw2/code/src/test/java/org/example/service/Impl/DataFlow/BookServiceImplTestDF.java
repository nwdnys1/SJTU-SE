package org.example.service.Impl.DataFlow;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.mockito.Mockito.*;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
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
class BookServiceImplTestDF {

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
		Book bookWithDetails = new Book();
		bookWithDetails.setId(1);
		bookWithDetails.setBookDetails(new BookDetails());

		Book bookWithComments = new Book();
		bookWithComments.setId(2);
		bookWithComments.setBookDetails(new BookDetails());

		Book bookWithTags = new Book();
		bookWithTags.setId(3);
		bookWithTags.setBookDetails(new BookDetails());

		return Stream.of(Arguments.of(1, bookWithDetails, 0, Collections.emptyList(), 200, "验证书籍详情加载"),
				Arguments.of(2, bookWithComments, 5, Collections.emptyList(), 200, "验证评论加载"),
				Arguments.of(3, bookWithTags, 0, Arrays.asList("Tag1", "Tag2"), 200, "验证标签加载"),
				Arguments.of(999, null, 0, Collections.emptyList(), 404, "验证书籍不存在"));
	}

	// ========== updateBook 测试 ==========
	static Stream<Arguments> provideUpdateBookTestCases() {
		Book existingBook = new Book();
		existingBook.setId(1);
		existingBook.setTitle("Old Title");
		existingBook.setAuthor("Old Author");
		existingBook.setIsbn("Old ISBN");
		existingBook.setBookDetails(new BookDetails());
		existingBook.getBookDetails().setStock(10);

		Book updateData = new Book();
		updateData.setTitle("New Title");
		updateData.setAuthor("New Author");
		updateData.setIsbn("New ISBN");
		updateData.setBookDetails(new BookDetails());
		updateData.getBookDetails().setStock(20);

		return Stream.of(Arguments.of(1, existingBook, updateData, 200, "验证标题更新"),
				Arguments.of(1, existingBook, updateData, 200, "验证作者更新"),
				Arguments.of(1, existingBook, updateData, 200, "验证ISBN更新"),
				Arguments.of(1, existingBook, updateData, 200, "验证库存更新"),
				Arguments.of(999, null, updateData, 404, "验证书籍不存在"));
	}
	// ========== deleteBook 测试 ==========
	static Stream<Arguments> provideDeleteBookTestCases() {
		return Stream.of(Arguments.of(1, true, false, false, 200, "可删除书籍"),
				Arguments.of(999, false, false, false, 404, "书籍不存在"),
				Arguments.of(2, true, true, false, 400, "书籍在购物车中"), Arguments.of(3, true, false, true, 400, "书籍已被购买"));
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
	void testGetBookById(int id, Book mockBook, int commentCount, List<Tag> tags, int expectedStatus,
			String description) {
		when(bookDAO.getBookById(id)).thenReturn(mockBook);
		if (mockBook != null) {
			when(bookDAO.getBookDetailsById(id)).thenReturn(mockBook != null ? mockBook.getBookDetails() : null);
			when(commentDAO.getCommentsByBookId(id)).thenReturn(Collections.nCopies(commentCount, null));
			when(bookTagRepository.findTidsByBid(id)).thenReturn(Collections.emptyList());
			when(tagRepository.findTagNamesByTids(anyList())).thenReturn(tags);
		}

		Result<Book> result = bookService.getBookById(id);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertNotNull(result.getData());
			assertEquals(id, result.getData().getId());
			assertEquals(commentCount, result.getData().getBookDetails().getComments().size());
			assertEquals(tags.size(), result.getData().getTags().size());
		} else {
			assertEquals(404, result.getCode());
			assertEquals("书籍不存在！", result.getMessage());
		}

	}

	// ========== getRecommendations 测试 ==========
	@Test
	void testGetRecommendations() {
		List<Book> mockBooks = new ArrayList<>();
		for (int i = 1; i <= 10; i++) {
			Book book = new Book();
			book.getBookDetails().setRating(i);
			mockBooks.add(book);
		}
		when(bookDAO.getBooksByRatingGreaterThanOrderByRatingDesc(0)).thenReturn(mockBooks);

		Result<List<Book>> result = bookService.getRecommendations(5);

		assertEquals(200, result.getCode());
		assertEquals(5, result.getData().size());
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
	void testUpdateBook(int id, Book existingBook, Book updateData, int expectedStatus, String description) {
		when(bookDAO.existsById(id)).thenReturn(existingBook != null);
		if (existingBook != null) {
			when(bookDAO.getBookById(id)).thenReturn(existingBook);
			when(bookDAO.save(any(Book.class))).thenReturn(existingBook);
		}

		Result<Book> result = bookService.updateBook(id, updateData);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertEquals(id, result.getData().getId());
			assertEquals("New Title", result.getData().getTitle());
			assertEquals("New Author", result.getData().getAuthor());
			assertEquals("New ISBN", result.getData().getIsbn());
			assertEquals(20, result.getData().getBookDetails().getStock());
		} else if (expectedStatus == 404) {
			assertEquals(404, result.getCode());
			assertEquals("书籍不存在！", result.getMessage());
		}
	}

	// ========== deleteBook 测试 ==========
	@ParameterizedTest
	@MethodSource("provideDeleteBookTestCases")
	void testDeleteBook(int id, boolean bookExists, boolean inCart, boolean purchased, int expectedStatus,
			String description) {
		when(bookDAO.existsById(id)).thenReturn(bookExists);
		if (bookExists) {
			when(cartItemDAO.existsByBookId(id)).thenReturn(inCart);
			if (!inCart) {
				when(orderItemDAO.existsByBookId(id)).thenReturn(purchased);
			}
		}

		if (bookExists && !inCart && !purchased) {
			doNothing().when(bookDAO).deleteById(id);
		}
		Result<Book> result = bookService.deleteBook(id);
		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			verify(bookDAO).deleteById(id);
		} else if (expectedStatus == 404) {
			verify(bookDAO, never()).deleteById(id);
		} else if (expectedStatus == 400) {
			verify(bookDAO, never()).deleteById(id);
		}
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

	// ========== updateCover 测试 ==========
	@ParameterizedTest
	@MethodSource("provideUpdateCoverTestCases")
	void testUpdateCover(int id, Book existingBook, MultipartFile file, String coverUrl, int expectedStatus,
			String description) throws IOException {
		when(bookDAO.getBookById(id)).thenReturn(existingBook);
		if (existingBook != null) {
			when(uploadRepository.uploadFile(file, "image")).thenReturn(coverUrl);
			when(bookDAO.save(any(Book.class))).thenReturn(existingBook);
		}

		uploadRepository.uploadFile(file, "image");

		Result<String> result = bookService.updateCover(id, file);

		assertEquals(expectedStatus, result.getCode());
		if (expectedStatus == 200) {
			assertEquals(coverUrl, result.getData());
			verify(bookDAO).save(existingBook);
		} else if (expectedStatus == 404) {
			verify(bookDAO, never()).save(existingBook);
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