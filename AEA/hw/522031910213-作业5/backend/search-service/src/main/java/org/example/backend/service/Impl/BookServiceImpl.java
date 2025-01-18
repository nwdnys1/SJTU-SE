package org.example.backend.service.Impl;

import java.util.List;

import org.example.backend.DAO.BookDAO;
import org.example.backend.entity.Book;
import org.example.backend.entity.Result;
import org.example.backend.service.BookService;
import org.springframework.stereotype.Service;

@Service
public class BookServiceImpl implements BookService {
	private final BookDAO repository;
	public BookServiceImpl(BookDAO repository) {
		this.repository = repository;
	}

	@Override
	public Result<List<String>> searchAuthorByTitle(String title) {
		List<Book> books = repository.findByTitle(title);
		if (books.isEmpty()) {
			return Result.error(404, "找不到书籍！");
		}
		List<String> authors = books.stream().map(Book::getAuthor).toList();
		return Result.success(authors);
	}
}
