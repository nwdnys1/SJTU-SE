package org.example.rpa.repo;

import org.example.rpa.entity.Invoice;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface InvoiceRepo extends JpaRepository<Invoice, Integer> {
    public Invoice getInvoiceByNumber(String number);
}
