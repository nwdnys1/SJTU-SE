package org.example.DAO.Impl;

import java.util.List;

import org.example.DAO.BookDAO;
import org.example.entity.Book;
import org.example.entity.BookDetails;
import org.example.repository.BookDetailsRepository;
import org.example.repository.BookRepository;
import org.example.repository.BookTagRepository;
import org.example.utils.RedisUtil;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageImpl;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;
@Repository
public class BookDAOImpl implements BookDAO {
	private final BookRepository repository;
	private final BookDetailsRepository bookDetailsRepository;
	private final BookTagRepository bookTagRepository;
	private final RedisUtil redisUtil;

	public BookDAOImpl(BookRepository repository, RedisUtil redisUtil, BookDetailsRepository bookDetailsRepository,
			BookTagRepository bookTagRepository) {
		this.repository = repository;
		this.redisUtil = redisUtil;
		this.bookDetailsRepository = bookDetailsRepository;
		this.bookTagRepository = bookTagRepository;
		redisUtil.setPREFIX("book::");
	}
	@Override
	public Book getBookById(int id) {
		try {
			Book book = (Book) redisUtil.get("id::" + id);
			if (book == null) {
				book = repository.findById(id).orElse(null);
				System.out.println(book.toString());
				redisUtil.set("id::" + id, book);
			}
			return book;
		} catch (Exception e) { // redis error or other error return from MySQL
			System.out.println("Redis error: " + e.getMessage());
			return repository.findById(id).orElse(null);
		}
	};
	@Override
	public BookDetails getBookDetailsById(int id) {
		return bookDetailsRepository.findById(id).orElse(null);
	};
	@Override
	public List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(int rating) {
		return repository.getBooksByBookDetailsRatingGreaterThanOrderByBookDetailsRatingDesc(rating);
	};
	@Override
	public Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable) {
		return repository.getBooksByTitleLikeOrAuthorLike(title, author, pageable);
	};
	@Override
	public Page<Book> findBooksByTagIds(List<Long> tids, Pageable pageable) {
		// 查询所有与指定 tid 相关的 bid（分页查询）
		Page<Integer> bidsPage = bookTagRepository.findBidsByTids(tids, pageable);
		// 使用返回的 bid 列表查询所有关联的 Book
		if (bidsPage.hasContent()) {
			List<Book> books = repository.findAllById(bidsPage.getContent());// 通过id查询所有的book
			return new PageImpl<>(books, pageable, bidsPage.getTotalElements());// 返回分页
		} else {
			return Page.empty(); // 如果没有相关 book，返回空分页
		}
	};
	@Override
	public boolean existsById(int id) {
		return repository.existsById(id);
	}
	@Override
	public Book save(Book book) {
		book = repository.save(book);// 先保存到数据库 获取id
		redisUtil.set("id::" + book.getId(), book);
		System.out.println("Save book to redis");
		return book;
	}
	@Override
	@Transactional
	public BookDetails saveDetails(BookDetails bookDetails) {
		return bookDetailsRepository.save(bookDetails);
	}
	@Override
	public void deleteById(int id) {
		redisUtil.del("id::" + id);
		System.out.println("Delete book from redis");
		repository.deleteById(id);
	}
}
