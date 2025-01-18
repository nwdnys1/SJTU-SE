package org.example.backend.DAO.Impl;

import org.example.backend.DAO.BookDAO;
import org.example.backend.entity.Book;
import org.example.backend.entity.BookDetails;
import org.example.backend.entity.Tag;
import org.example.backend.repository.MySQLRepository.BookDetailsRepository;
import org.example.backend.repository.MySQLRepository.BookRepository;
import org.example.backend.utils.RedisUtil;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.math.BigDecimal;
import java.util.List;
@Repository
public class BookDAOImpl implements BookDAO {
    private final BookRepository repository;
    private final BookDetailsRepository bookDetailsRepository;
    private final RedisUtil redisUtil;

    public BookDAOImpl(BookRepository repository, RedisUtil redisUtil, BookDetailsRepository bookDetailsRepository) {
        this.repository = repository;
        this.redisUtil = redisUtil;
        this.bookDetailsRepository = bookDetailsRepository;
        redisUtil.setPREFIX("book::");
    }
    @Override
    public Book getBookById(int id){
        Book book = (Book) redisUtil.get("id::" + id);
        if (book == null) {
            book = repository.findById(id).orElse(null);
            redisUtil.set("id::" + id, book);
            System.out.println("Get book from MySQL, and save to redis");
        }
        else {
            System.out.println("Get book from redis");
            book.setBookDetails(bookDetailsRepository.findById(id).orElse(null));
        }
        return book;
    };
    @Override
    public BookDetails getBookDetailsById(int id){
        return bookDetailsRepository.findById(id).orElse(null);
    };
    @Override
    public List<Book> getBooksByRatingGreaterThanOrderByRatingDesc(int rating){
        return repository.getBooksByBookDetailsRatingGreaterThanOrderByBookDetailsRatingDesc(rating);
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
    public boolean existsById(int id) {
        return repository.existsById(id);
    }
    @Override
    public Book save(Book book) {
        book = repository.save(book);//先保存到数据库 获取id
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
