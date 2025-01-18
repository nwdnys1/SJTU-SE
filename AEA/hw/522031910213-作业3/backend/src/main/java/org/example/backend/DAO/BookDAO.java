package org.example.backend.DAO;

import org.example.backend.entity.Book;
import org.example.backend.entity.Tag;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;

import java.math.BigDecimal;
import java.util.List;

public interface BookDAO {
    Book getBookById(int id);

    List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(BigDecimal rating);

    Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable);

    Page<Book> findBooksByTagsContains(Tag tag, Pageable pageable);

    List<Book> findAll();

    boolean existsById(int id);

    Book save(Book book);

    Book findById(int id);

    void deleteById(int id);
}
