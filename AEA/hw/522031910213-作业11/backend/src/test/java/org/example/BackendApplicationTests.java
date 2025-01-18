package org.example;

import jakarta.annotation.Resource;
import org.example.repository.CartItemRepository;
import org.example.service.CartItemService;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;

@SpringBootTest
class BackendApplicationTests {
    @Resource
    CartItemRepository cartItemRepository;
    CartItemService cartItemService;

    @Test
    void contextLoads() {

            BCryptPasswordEncoder encoder = new BCryptPasswordEncoder();
            System.out.print(encoder.encode("123"));


    }

}
