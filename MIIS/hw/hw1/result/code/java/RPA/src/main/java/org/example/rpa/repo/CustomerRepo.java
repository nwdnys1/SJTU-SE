package org.example.rpa.repo;

import org.example.rpa.entity.Customer;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface CustomerRepo extends JpaRepository<Customer, Integer> {
    public Customer getCustomerByName(String name);
    public List<Customer> getCustomersByType(String type);

}
