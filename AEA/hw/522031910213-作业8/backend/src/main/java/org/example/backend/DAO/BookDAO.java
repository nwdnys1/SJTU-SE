package org.example.backend.DAO;

import java.util.List;

import org.example.backend.entity.Book;
import org.example.backend.entity.BookDetails;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;

public interface BookDAO {
	Book getBookById(int id);

	BookDetails getBookDetailsById(int id);

	List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(int rating);

	Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable);

	Page<Book> findBooksByTagIds(List<Long> tids, Pageable pageable);

	boolean existsById(int id);

	Book save(Book book);

	BookDetails saveDetails(BookDetails bookDetails);

	void deleteById(int id);
}
