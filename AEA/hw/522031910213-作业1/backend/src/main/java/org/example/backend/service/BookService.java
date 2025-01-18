package org.example.backend.service;

import org.example.backend.DTO.PageResponse;
import org.example.backend.DTO.SalesInfo;
import org.example.backend.entity.Book;
import org.example.backend.entity.Result;
import org.springframework.web.multipart.MultipartFile;

import java.time.LocalDateTime;
import java.util.List;
public interface BookService {
    public Result<List<Book>> getBooks();
    public Result<Book> getBookById(int id);
    public Result<List<Book>> getRecommendations(int nums);
    public Result<Book> addBook(Book book);
    public Result<Book> updateBook(int id, Book book);
    public Result<Book> deleteBook(int id);
    public Result<PageResponse<Book>> searchBooks(String keyword, int page, int pageSize);
    public Result<PageResponse<Book>> categorySearch(int tid, int page, int pageSize);
    public Result<String> updateCover(int id, MultipartFile file);
    public Result<List<SalesInfo>> rank(LocalDateTime start, LocalDateTime end, int nums);
}

