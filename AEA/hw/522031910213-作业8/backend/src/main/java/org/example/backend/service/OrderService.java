package org.example.backend.service;

import org.example.backend.DTO.PageResponse;
import org.example.backend.entity.CartItem;
import org.example.backend.entity.Order;
import org.example.backend.entity.OrderRequest;
import org.example.backend.entity.Result;

import java.time.LocalDateTime;
import java.util.List;

public interface OrderService {
    Result<List<Order>> getOrders();

    Result<Order> addOrder(OrderRequest request);

    // 在购物车项列表中查找指定ID的购物车项
    CartItem findCartItemById(List<CartItem> cartItems, int cid);

    Result<Order> deleteOrder(int id);

    Result<List<Order>> getAllOrders();

    Result<PageResponse<Order>> searchAllOrders(String keyword, int page, int pageSize, LocalDateTime start, LocalDateTime end);

    Result<PageResponse<Order>> searchOrders(String keyword, LocalDateTime start, LocalDateTime end, int page, int pageSize);
}
