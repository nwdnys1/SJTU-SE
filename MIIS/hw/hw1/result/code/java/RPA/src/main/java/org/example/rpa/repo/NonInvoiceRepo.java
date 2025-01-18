package org.example.rpa.repo;

import org.example.rpa.entity.NonInvoice;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface NonInvoiceRepo extends JpaRepository<NonInvoice, Integer>{
}
