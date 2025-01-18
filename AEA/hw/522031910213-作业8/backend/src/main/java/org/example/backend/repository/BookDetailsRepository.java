package org.example.backend.repository;

import org.example.backend.entity.BookDetails;
import org.springframework.data.jpa.repository.JpaRepository;

public interface BookDetailsRepository extends JpaRepository<BookDetails, Integer> {

}
