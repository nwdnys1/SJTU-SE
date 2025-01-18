package org.example.backend.DAO;

import java.util.List;

import org.example.backend.entity.Book;

public interface BookDAO {
	List<Book> findByTitle(String title);
}
