package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.CartItem;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

public interface CartItemRepository extends JpaRepository<CartItem, Integer> {
    public List<CartItem> getCartItemsByUserId(int uid);
    public CartItem getCartItemById(int id);
    public boolean existsByUserIdAndBookId(int uid, int bid);
}
