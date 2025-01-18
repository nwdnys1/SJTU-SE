package org.example.backend.service;

import org.example.backend.entity.Result;

import java.util.List;

public interface BookService {
	Result<List<String>> searchAuthorByTitle(String title);
}
