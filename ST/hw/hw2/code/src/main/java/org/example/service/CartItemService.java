package org.example.service;

import org.example.entity.CartItem;
import org.example.entity.Result;

import java.util.List;

public interface CartItemService {
    Result<List<CartItem>> getCartItems();

    Result<CartItem> addCartItem(int bid);

    Result<CartItem> deleteCartItem(int id);

    Result<CartItem> updateCartItem(int id, int quantity);
}
