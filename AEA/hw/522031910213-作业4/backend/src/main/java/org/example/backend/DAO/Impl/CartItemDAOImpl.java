package org.example.backend.DAO.Impl;

import org.example.backend.DAO.CartItemDAO;
import org.example.backend.entity.CartItem;
import org.example.backend.repository.MySQLRepository.CartItemRepository;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
@Repository
public class CartItemDAOImpl implements CartItemDAO {
    private final CartItemRepository repository;
    public CartItemDAOImpl(CartItemRepository repository) {
        this.repository = repository;
    }
    @Override
    public List<CartItem> getCartItemsByUserId(int uid){
        return repository.getCartItemsByUserId(uid);
    };
    @Override
    public CartItem getCartItemById(int id){
        return repository.getCartItemById(id);
    };
    @Override
    public boolean existsByUserIdAndBookId(int uid, int bid){
        return repository.existsByUserIdAndBookId(uid, bid);
    };

    @Override
    public void save(CartItem item) {
        repository.save(item);
    }

    @Override
    public boolean existsById(int id) {
        return repository.existsById(id);
    }
    @Override
    public boolean existsByBookId(int id) {
        return repository.existsByBookId(id);
    }

    @Override
    @Transactional
    public void deleteById(int id) {
        repository.deleteById(id);
    }
}
