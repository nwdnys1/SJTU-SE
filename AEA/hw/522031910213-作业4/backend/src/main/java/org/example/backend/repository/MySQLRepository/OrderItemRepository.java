package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.OrderItem;
import org.springframework.data.jpa.repository.JpaRepository;

public interface OrderItemRepository extends JpaRepository<OrderItem, Integer> {
    boolean existsByBookId(int id);
}
