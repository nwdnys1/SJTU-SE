package org.example.backend.service.Impl;

import org.example.backend.DAO.CartItemDAO;
import org.example.backend.DAO.UserDAO;
import org.example.backend.entity.Book;
import org.example.backend.entity.CartItem;
import org.example.backend.entity.Result;
import org.example.backend.entity.User;
import org.example.backend.service.CartItemService;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class CartItemServiceImpl implements CartItemService {
    private final CartItemDAO repository;
    private final UserDAO userDAO;
    public CartItemServiceImpl(CartItemDAO repository, UserDAO userDAO) {
        this.repository = repository;
        this.userDAO = userDAO;
    }
    public int getUid() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.findUserByUsername(username).getId();
    }
    @Override
    public Result<List<CartItem>> getCartItems(){
        return Result.success(repository.getCartItemsByUserId(getUid()));
    }
    @Override
    public Result<CartItem> addCartItem(int bid){//需要判断是否已经存在
        int uid = getUid();
        if(repository.existsByUserIdAndBookId(uid, bid)){
            return Result.error(400, "购物车项目已存在！");
        }else{
            CartItem item = new CartItem();
            item.setQuantity(1);
            item.setUser(new User());
            item.getUser().setId(uid);
            item.setBook(new Book());
            item.getBook().setId(bid);
            repository.save(item);
            return Result.success(item);
        }
    }
    @Override
    public Result<CartItem> deleteCartItem(int id){//需要判断是否存在
        if(repository.existsById(id)){
            if(repository.getCartItemById(id).getUser().getId()!=getUid()){
                return Result.error(403, "无权操作！");
            }
            repository.deleteById(id);
            return Result.success(null);
        }else{
            return Result.error(404, "购物车项目不存在！");
        }
    }
    @Override
    public Result<CartItem> updateCartItem(int id, int quantity){//需要判断是否存在
        if(repository.existsById(id)){
            CartItem item = repository.getCartItemById(id);
            if(item.getUser().getId()!=getUid()){
                return Result.error(403, "无权操作！");
            }
            item.setQuantity(quantity);
            repository.save(item);
            return Result.success(item);
        }else{
            return Result.error(404, "购物车项目不存在！");
        }
    }
}
