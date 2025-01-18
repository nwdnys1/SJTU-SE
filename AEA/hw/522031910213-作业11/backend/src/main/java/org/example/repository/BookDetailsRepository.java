package org.example.repository;

import org.example.entity.BookDetails;
import org.springframework.data.jpa.repository.JpaRepository;

public interface BookDetailsRepository extends JpaRepository<BookDetails, Integer> {

}
