package org.example.repository;

import org.example.entity.OrderItem;
import org.springframework.data.jpa.repository.JpaRepository;

public interface OrderItemRepository extends JpaRepository<OrderItem, Integer> {
    boolean existsByBookId(int id);
}
