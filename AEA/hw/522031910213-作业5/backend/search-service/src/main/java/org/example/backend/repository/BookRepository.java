package org.example.backend.repository;

import java.util.List;

import org.example.backend.entity.Book;
import org.springframework.data.jpa.repository.JpaRepository;

public interface BookRepository extends JpaRepository<Book, Integer> {
	List<Book> findByTitle(String title);
}
