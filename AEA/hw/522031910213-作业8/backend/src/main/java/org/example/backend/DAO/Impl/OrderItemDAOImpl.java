package org.example.backend.DAO.Impl;

import org.example.backend.DAO.OrderItemDAO;
import org.example.backend.entity.OrderItem;
import org.example.backend.repository.OrderItemRepository;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
@Repository
public class OrderItemDAOImpl implements OrderItemDAO {
    private final OrderItemRepository repository;
    public OrderItemDAOImpl(OrderItemRepository repository) {
        this.repository = repository;
    }
    @Override//保存所有订单项
    @Transactional
    public void saveAll(List<OrderItem> orderItems){
        repository.saveAll(orderItems);
    }
    @Override
    public boolean existsByBookId(int id){
        return repository.existsByBookId(id);
    }
}
