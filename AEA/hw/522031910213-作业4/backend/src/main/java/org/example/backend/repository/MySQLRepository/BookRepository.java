package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.Book;
import org.example.backend.entity.Tag;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.math.BigDecimal;
import java.util.List;

public interface BookRepository extends JpaRepository<Book, Integer> {
    public List<Book> getBooksByBookDetailsRatingGreaterThanOrderByBookDetailsRatingDesc(int rating);
    public Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable);
    public Page<Book> findBooksByTagsContains(Tag tag, Pageable pageable);
}
