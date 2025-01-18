package org.example.backend.service;

import java.time.LocalDateTime;
import java.util.List;

import org.example.backend.DTO.PageResponse;
import org.example.backend.DTO.SalesInfo;
import org.example.backend.entity.Book;
import org.example.backend.entity.Result;
import org.springframework.web.multipart.MultipartFile;
public interface BookService {
	Result<Book> getBookById(int id);
	Result<List<Book>> getRecommendations(int nums);
	Result<Book> addBook(Book book);
	Result<Book> updateBook(int id, Book book);
	Result<Book> deleteBook(int id);
	Result<PageResponse<Book>> searchBooks(String keyword, int page, int pageSize);
	Result<String> updateCover(int id, MultipartFile file);
	Result<List<SalesInfo>> rank(LocalDateTime start, LocalDateTime end, int nums);
	Result<PageResponse<Book>> searchBookByTagIds(List<Long> tags, int page, int pageSize);
}
