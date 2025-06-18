package org.example.DAO;


import org.example.entity.OrderItem;

import java.util.List;

public interface OrderItemDAO {
    void saveAll(List<OrderItem> orderItems);
    boolean existsByBookId(int id);
}
