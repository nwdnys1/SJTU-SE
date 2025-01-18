package org.example.backend.repository;

import org.example.backend.entity.CartItem;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface CartItemRepository extends JpaRepository<CartItem, Integer> {
    List<CartItem> getCartItemsByUserId(int uid);
    CartItem getCartItemById(int id);
    boolean existsByUserIdAndBookId(int uid, int bid);

    boolean existsByBookId(int id);
}
