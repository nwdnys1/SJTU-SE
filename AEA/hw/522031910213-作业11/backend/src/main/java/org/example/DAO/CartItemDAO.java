package org.example.DAO;

import org.example.entity.CartItem;

import java.util.List;

public interface CartItemDAO {
    List<CartItem> getCartItemsByUserId(int uid);

    CartItem getCartItemById(int id);

    boolean existsByUserIdAndBookId(int uid, int bid);

    void save(CartItem item);

    boolean existsById(int id);
    boolean existsByBookId(int id);

    void deleteById(int id);
}
