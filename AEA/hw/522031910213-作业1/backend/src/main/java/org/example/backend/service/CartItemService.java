package org.example.backend.service;

import org.example.backend.entity.CartItem;
import org.example.backend.entity.Result;

import java.util.List;

public interface CartItemService {
    Result<List<CartItem>> getCartItems();

    Result<CartItem> addCartItem(int bid);

    Result<CartItem> deleteCartItem(int id);

    Result<CartItem> updateCartItem(int id, int quantity);
}
