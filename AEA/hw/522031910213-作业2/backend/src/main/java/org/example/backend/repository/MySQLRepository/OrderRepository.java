package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.Order;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDateTime;
import java.util.List;

public interface OrderRepository extends JpaRepository<Order, Integer> {
    public List<Order> getOrdersByUserId(int uid);
    public Order getOrderById(int id);
    public List<Order> getOrdersByOrderItemsBookTitleLikeAndUserId(String title, int uid);
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid, Pageable pageable);
    public List<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(LocalDateTime start, LocalDateTime end, int uid);//用于找到某个用户在某个时间段内的订单
    public Page<Order> getOrdersByOrderItemsBookTitleLike(String s, Pageable pageable);
    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(LocalDateTime start, LocalDateTime end, String title, int uid, Pageable pageable);

    public Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(LocalDateTime start, LocalDateTime end, String s, Pageable pageable);

    Page<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end, Pageable pageable);
    List<Order> getOrdersByCreateTimeAfterAndCreateTimeBefore(LocalDateTime start, LocalDateTime end);//用于找到所有时间段内的订单
}