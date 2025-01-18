package org.example.rpa.repo;

import org.example.rpa.entity.Frequency;
import org.springframework.data.jpa.repository.JpaRepository;

public interface FrequencyRepo extends JpaRepository<Frequency, Integer> {
    public Frequency getFrequencyByCustomerIdAndSupplierId(Integer customerId, Integer supplierId);
}
