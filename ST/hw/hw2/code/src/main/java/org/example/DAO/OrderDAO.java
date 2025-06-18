package org.example.DAO;

import org.example.entity.Order;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;

import java.time.LocalDateTime;
import java.util.List;

public interface OrderDAO {
    List<Order> getOrdersByUserId(int uid);
    Order getOrderById(int id);
    List<Order> getOrdersByOrderItemsBookTitleLikeAndUserId(String title, int uid);
    Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid, Pageable pageable);
    List<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid);//用于找到某个用户在某个时间段内的订单
    Page<Order> getOrdersByOrderItemsBookTitleLike(String s, Pageable pageable);
    Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(LocalDateTime start, LocalDateTime end, String title, int uid, Pageable pageable);
    Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(LocalDateTime start, LocalDateTime end, String s, Pageable pageable);
    Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end, Pageable pageable);
    List<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end);//用于找到所有时间段内的订单
    void save(Order order);
    boolean existsById(int id);
    void deleteById(int id);
    List<Order> findAll();
}
