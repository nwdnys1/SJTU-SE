package org.example.backend.repository;

import org.example.backend.entity.Book;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface BookRepository extends JpaRepository<Book, Integer> {
    public List<Book> getBooksByBookDetailsRatingGreaterThanOrderByBookDetailsRatingDesc(int rating);
    public Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable);

}
