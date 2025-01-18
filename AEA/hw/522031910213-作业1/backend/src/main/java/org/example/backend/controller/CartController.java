package org.example.backend.controller;

import org.example.backend.entity.*;
import org.example.backend.service.CartItemService;
import org.springframework.web.bind.annotation.*;
import java.util.List;

@RestController
@RequestMapping("/api/cart")
public class CartController {
    private final CartItemService service;
    public CartController(CartItemService service) {
        this.service = service;
    }

    @GetMapping("/list")
    public Result<List<CartItem>> getCartItems() {

        return service.getCartItems();
    }
    @PostMapping("/add/{bid}")
    public Result<CartItem> addCartItem(@PathVariable int bid) {

        return service.addCartItem(bid);
    }
    @DeleteMapping("/delete/{id}")
    public Result<CartItem> deleteCartItem(@PathVariable int id) {
        return service.deleteCartItem(id);
    }
    @PutMapping("/update/{id}")
    public Result<CartItem> updateCartItem(@PathVariable int id, @RequestParam int quantity) {
        return service.updateCartItem(id, quantity);
    }
}
