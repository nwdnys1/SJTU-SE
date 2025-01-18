package org.example.backend.controller;

import java.util.List;

import org.example.backend.entity.Result;
import org.example.backend.service.BookService;
import org.springframework.web.bind.annotation.*;

@RestController
@RequestMapping("/api/search")
public class BookController {
	private final BookService service;
	public BookController(BookService service) {
		this.service = service;
	}
	@GetMapping("/author")
	@CrossOrigin(origins = "http://localhost:5173",allowCredentials = "true")
	public Result<List<String>> searchAuthor(@RequestParam String title) {
		return service.searchAuthorByTitle(title);
	}
}
