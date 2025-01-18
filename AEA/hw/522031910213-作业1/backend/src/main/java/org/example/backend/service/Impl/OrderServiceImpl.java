package org.example.backend.service.Impl;

import jakarta.transaction.Transactional;
import org.example.backend.DAO.BookDAO;
import org.example.backend.DAO.CartItemDAO;
import org.example.backend.DAO.OrderDAO;
import org.example.backend.DAO.UserDAO;
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

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

@Service
public class OrderServiceImpl implements OrderService {
    private final OrderDAO repository;
    private final CartItemDAO cartItemRepository;
    private final UserDAO userDAO;
    private final BookDAO bookDAO;
    public OrderServiceImpl(OrderDAO repository, CartItemDAO cartItemRepository, UserDAO userDAO, BookDAO bookDAO) {
        this.repository = repository;
        this.cartItemRepository = cartItemRepository;
        this.userDAO = userDAO;
        this.bookDAO = bookDAO;
    }
    public int getUid() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.findUserByUsername(username).getId();
    }
    @Override
    public Result<List<Order>> getOrders(){
        return Result.success(repository.getOrdersByUserId(getUid()));
    }
    @Override
    @Transactional
    public Result<Order> addOrder(OrderRequest request){
        int uid = request.getUid();
        Order order = new Order();
        order.setAddress(request.getAddress());
        order.setTel(request.getTel());
        order.setReceiver(request.getReceiver());
        order.setCreateTime(LocalDateTime.now());
        order.setUser(new User());
        order.getUser().setId(uid);
        List<OrderItem> orderItems = new ArrayList<>();

        // 一次性从购物车中获取所有商品项
        List<CartItem> cartItems = cartItemRepository.getCartItemsByUserId(uid);
        for (int cid:request.getCids()){
            CartItem cartItem = findCartItemById(cartItems, cid);
            if(cartItem == null){
                return Result.error(400, "购物车项目不存在！");
            }
        }
        for (int cid : request.getCids()) {
            // 在购物车项中查找对应的商品项
            CartItem cartItem = findCartItemById(cartItems, cid);
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
            cartItems.remove(cartItem);
            cartItemRepository.deleteById(cartItem.getId());
        }
        order.setOrderItems(orderItems);
        repository.save(order);
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
        if(repository.existsById(id)){
            if(repository.getOrderById(id).getUser().getId() != getUid()){
                return Result.error(403, "无权删除他人订单！");
            }
            repository.deleteById(id);
            return Result.success(null);
        }else{
            return Result.error(404, "订单不存在！");
        }
    }
    @Override
    public Result<List<Order>> getAllOrders() {
        return Result.success(repository.findAll());
    }
    @Override
    public Result<PageResponse<Order>> searchAllOrders(String keyword, int page, int pageSize, LocalDateTime start, LocalDateTime end){
        Pageable pageable = PageRequest.of(page - 1, pageSize);
        Page<Order> orders;
        if(keyword.isEmpty())
            orders = repository.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end, pageable);
        else
            orders = repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLike(start, end, "%" + keyword + "%", pageable);
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
            orders = repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end, getUid(), pageable);
        else
            orders = repository.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndOrderItemsBookTitleLikeAndUserId(start, end, "%" + keyword + "%", getUid(), pageable);
        PageResponse<Order> response = new PageResponse<Order>(
                orders.getTotalElements(),
                orders.getTotalPages(),
                orders.getContent()
        );
        return Result.success(response);
    }
}
