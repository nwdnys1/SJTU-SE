package org.example.controller;

import java.util.List;

import org.example.entity.Book;
import org.example.service.BookService;
import org.springframework.graphql.data.method.annotation.Argument;
import org.springframework.graphql.data.method.annotation.QueryMapping;
import org.springframework.stereotype.Controller;

@Controller
public class GraphqlController {
	private final BookService service;

	public GraphqlController(BookService service) {
		this.service = service;
	}
	@QueryMapping
	public List<Book> findBooksByTitle(@Argument String title) {
		return service.searchBooks(title, 1, 100).getData().getContent();
	}
}
