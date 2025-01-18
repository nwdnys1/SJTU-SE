package org.example.backend.service.Impl;

import org.example.backend.DAO.*;
import org.example.backend.DTO.PageResponse;
import org.example.backend.entity.OrderRequest;
import org.example.backend.entity.*;
import org.example.backend.service.OrderService;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

@Service
public class OrderServiceImpl implements OrderService {
    private final OrderDAO orderDAO;
    private final OrderItemDAO orderItemDAO;
    private final CartItemDAO cartItemDAO;
    private final UserDAO userDAO;
    private final BookDAO bookDAO;
    public OrderServiceImpl(OrderDAO orderDAO, CartItemDAO cartItemDAO, UserDAO userDAO, BookDAO bookDAO, OrderItemDAO orderItemDAO) {
        this.orderDAO = orderDAO;
        this.cartItemDAO = cartItemDAO;
        this.userDAO = userDAO;
        this.bookDAO = bookDAO;
        this.orderItemDAO = orderItemDAO;
    }
    public int getUid() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.findUserByUsername(username).getId();
    }
    @Override
    public Result<List<Order>> getOrders(){
        return Result.success(orderDAO.getOrdersByUserId(getUid()));
    }
    @Override
    @Transactional
    public Result<Order> addOrder(OrderRequest request){
        // 填写订单信息
        int uid = request.getUid();
        Order order = new Order();
        order.setAddress(request.getAddress());
        order.setTel(request.getTel());
        order.setReceiver(request.getReceiver());
        order.setCreateTime(LocalDateTime.now());
        order.setUser(new User());
        order.getUser().setId(uid);
        order.setUid(uid);//设置uid 用于消息队列传递
        List<OrderItem> orderItems = new ArrayList<>();

        // 根据cid查询购物车项 并检查是否属于当前用户
        List<CartItem> cartItems = new ArrayList<>();
        for (int cid : request.getCids()) {
            CartItem cartItem = cartItemDAO.getCartItemById(cid);
            if (cartItem == null) {
                return Result.error(400, "购物车项" + cid + "不存在！",order);
            }
            if (cartItem.getUser().getId() != uid) {
                return Result.error(400, "购物车项" + cid + "不属于您！",order);
            }
            cartItems.add(cartItem);
        }
        for (CartItem cartItem : cartItems) {
            int bid=cartItem.getBook().getId();
            // 创建订单项并设置到订单中
            OrderItem orderItem = new OrderItem();
            orderItem.setOrder(order);
            orderItem.setBook(new Book());
            orderItem.getBook().setId(bid);
            orderItem.setQuantity(cartItem.getQuantity());
            orderItems.add(orderItem);
            // 更新商品库存和销量
            Book book = bookDAO.getBookById(bid);
            if (book.getStock() < cartItem.getQuantity()) {
                return Result.error(400, "库存不足！");
            }
            book.setStock(book.getStock() - cartItem.getQuantity());
            book.setSales(book.getSales() + cartItem.getQuantity());
            bookDAO.save(book);
            // 从购物车中移除对应的商品项
            cartItemDAO.deleteById(cartItem.getId());
        }
        //order.setOrderItems(orderItems);
        orderDAO.save(order);//保存订单
        // int result = 10 / 0;
        orderItemDAO.saveAll(orderItems);//保存订单项
        // int result = 10 / 0;
        return Result.success(order);
    }

    // 在购物车项列表中查找指定ID的购物车项
    @Override
    public CartItem findCartItemById(List<CartItem> cartItems, int cid) {
        for (CartItem cartItem : cartItems) {
            if (cartItem.getId() == cid) {
                return cartItem;
            }
        }
        return null;
    }

    @Override
    public Result<Order> deleteOrder(int id){
        if(orderDAO.existsById(id)){
            if(orderDAO.getOrderById(id).getUser().getId() != getUid()){
                return Result.error(403, "无权删除他人订单！");
            }
            orderDAO.deleteById(id);
            return Result.success(null);
        }else{
            return Result.error(404, "订单不存在！");
        }
    }
    @Override
    public Result<List<Order>> getAllOrders() {
        return Result.success(orderDAO.findAll());
    }
    @Override
    public Result<PageResponse<Order>> searchAllOrders(String keyword, int page, int pageSize, LocalDateTime start, LocalDateTime end){
        Pageable pageable = PageRequest.of(page - 1, pageSize);
        Page<Order> orders;
        if(keyword.isEmpty())
            orders = orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, pageable);
        else
            orders = orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(start, end, "%" + keyword + "%", pageable);
        PageResponse<Order> response = new PageResponse<Order>(
                orders.getTotalElements(),
                orders.getTotalPages(),
                orders.getContent()
        );
        return Result.success(response);
    }
    @Override
    public Result<PageResponse<Order>> searchOrders(String keyword, LocalDateTime start, LocalDateTime end, int page, int pageSize){

        Pageable pageable = PageRequest.of(page - 1, pageSize);
        Page<Order> orders;
        if (keyword.isEmpty())//如果关键字为空 则只根据时间查询 这里是因为我发现当关键词为空时 会导致分页结果有问题 比如pageSize为6时只查询到2条 原因不明 只能先这样解决
            orders = orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, getUid(), pageable);
        else
            orders = orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(start, end, "%" + keyword + "%", getUid(), pageable);
        PageResponse<Order> response = new PageResponse<Order>(
                orders.getTotalElements(),
                orders.getTotalPages(),
                orders.getContent()
        );
        return Result.success(response);
    }
}
