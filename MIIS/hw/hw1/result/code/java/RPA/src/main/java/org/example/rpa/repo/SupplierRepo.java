package org.example.rpa.repo;

import org.example.rpa.entity.Supplier;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface SupplierRepo extends JpaRepository<Supplier, Integer> {
    public Supplier getSupplierByName(String name);
    public Supplier getSupplierByType(String type);
}
