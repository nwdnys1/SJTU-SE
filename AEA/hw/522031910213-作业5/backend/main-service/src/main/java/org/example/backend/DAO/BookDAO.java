package org.example.backend.DAO;

import org.example.backend.entity.Book;
import org.example.backend.entity.BookDetails;
import org.example.backend.entity.Tag;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;

import java.util.List;

public interface BookDAO {
    Book getBookById(int id);

    BookDetails getBookDetailsById(int id);

    List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(int rating);

    Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable);

    Page<Book> findBooksByTagsContains(Tag tag, Pageable pageable);

    boolean existsById(int id);

    Book save(Book book);

    BookDetails saveDetails(BookDetails bookDetails);

    void deleteById(int id);
}
