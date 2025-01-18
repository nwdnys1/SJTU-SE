package org.example.DAO.Impl;

import org.example.DAO.OrderDAO;
import org.example.entity.Order;
import org.example.repository.OrderRepository;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.List;
@Repository
public class OrderDAOImpl implements OrderDAO {
    private final OrderRepository repository;
    public OrderDAOImpl(OrderRepository repository) {
        this.repository = repository;
    }
    @Override
    public List<Order> getOrdersByUserId(int uid){
        return repository.getOrdersByUserId(uid);
    };
    @Override
    public Order getOrderById(int id){
        return repository.getOrderById(id);
    };
    @Override
    public List<Order> getOrdersByOrderItemsBookTitleLikeAndUserId(String title, int uid){
        return repository.getOrdersByOrderItemsBookTitleLikeAndUserId(title, uid);
    };
    @Override
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid, Pageable pageable){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, uid, pageable);
    };
    @Override
    public List<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, uid);
    };
    @Override
    public Page<Order> getOrdersByOrderItemsBookTitleLike(String s, Pageable pageable){
        return repository.getOrdersByOrderItemsBookTitleLike(s, pageable);
    };
    @Override
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(LocalDateTime start, LocalDateTime end, String title, int uid, Pageable pageable){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(start, end, title, uid, pageable);
    };

    @Override
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(LocalDateTime start, LocalDateTime end, String s, Pageable pageable){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(start, end, s, pageable);
    };

    @Override
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end, Pageable pageable){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, pageable);
    };
    @Override
    public List<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end){
        return repository.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end);
    };
    @Override
    @Transactional
    public void save(Order order) {
        repository.save(order);
    }

    @Override
    public boolean existsById(int id) {
        return repository.existsById(id);
    }

    @Override
    public void deleteById(int id) {
        repository.deleteById(id);
    }

    @Override
    public List<Order> findAll() {
        return repository.findAll();
    }
}
