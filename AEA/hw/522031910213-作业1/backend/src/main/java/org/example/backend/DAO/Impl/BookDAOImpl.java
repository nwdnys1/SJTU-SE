package org.example.backend.DAO.Impl;

import org.example.backend.DAO.BookDAO;
import org.example.backend.entity.Book;
import org.example.backend.entity.Tag;
import org.example.backend.repository.MySQLRepository.BookRepository;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Repository;

import java.math.BigDecimal;
import java.util.List;
@Repository
public class BookDAOImpl implements BookDAO {
    private final BookRepository repository;

    public BookDAOImpl(BookRepository repository) {
        this.repository = repository;
    }

    @Override
    public Book getBookById(int id){
        return repository.getBookById(id);
    };
    @Override
    public List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(BigDecimal rating){
        return repository.getBooksByRatingGreaterThanOrderByRatingDesc(rating);
    };
    @Override
    public Page<Book> getBooksByTitleLikeOrAuthorLike(String title, String author, Pageable pageable){
        return repository.getBooksByTitleLikeOrAuthorLike(title, author, pageable);
    };
    @Override
    public Page<Book> findBooksByTagsContains(Tag tag, Pageable pageable){
        return repository.findBooksByTagsContains(tag, pageable);
    };

    @Override
    public List<Book> findAll() {
        return repository.findAll();
    }

    @Override
    public boolean existsById(int id) {
        return repository.existsById(id);
    }

    @Override
    public Book save(Book book) {
        return repository.save(book);
    }

    @Override
    public Book findById(int id) {
        return repository.findById(id).orElse(null);
    }

    @Override
    public void deleteById(int id) {
        repository.deleteById(id);
    }
}
