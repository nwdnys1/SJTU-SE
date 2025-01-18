package org.example.backend.DAO;


import org.example.backend.entity.OrderItem;

import java.util.List;

public interface OrderItemDAO {
    void saveAll(List<OrderItem> orderItems);
    boolean existsByBookId(int id);
}
