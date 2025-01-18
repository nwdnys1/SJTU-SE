package org.example.service.Impl;

import org.example.DAO.BookDAO;
import org.example.DAO.CartItemDAO;
import org.example.DAO.UserDAO;
import org.example.entity.Book;
import org.example.entity.CartItem;
import org.example.entity.Result;
import org.example.entity.User;
import org.example.service.CartItemService;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Objects;

@Service
public class CartItemServiceImpl implements CartItemService {
    private final CartItemDAO repository;
    private final UserDAO userDAO;
    private final BookDAO bookDAO;
    public CartItemServiceImpl(CartItemDAO repository, UserDAO userDAO, BookDAO bookDAO) {
        this.repository = repository;
        this.userDAO = userDAO;
        this.bookDAO = bookDAO;
    }
    public User getUser() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.getUserByUsername(username);
    }
    @Override
    public Result<List<CartItem>> getCartItems(){
        return Result.success(repository.getCartItemsByUserId(getUser().getId()));
    }
    @Override
    public Result<CartItem> addCartItem(int bid){//需要判断是否已经存在
        User user = getUser();
        if(repository.existsByUserIdAndBookId(user.getId(), bid)){
            return Result.error(400, "购物车项目已存在！");
        }else{
            CartItem item = new CartItem();
            item.setQuantity(1);
            item.setUser(user);
            if (!bookDAO.existsById(bid)) {
                return Result.error(404, "书籍不存在！");
            }
            item.setBook(new Book());
            item.getBook().setId(bid);
            repository.save(item);
            return Result.success(item);
        }
    }
    @Override
    public Result<CartItem> deleteCartItem(int id){//需要判断是否存在
        if(repository.existsById(id)){
            if(!Objects.equals(repository.getCartItemById(id).getUser().getId(), getUser().getId())){
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
            if(!Objects.equals(item.getUser().getId(), getUser().getId())){
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
