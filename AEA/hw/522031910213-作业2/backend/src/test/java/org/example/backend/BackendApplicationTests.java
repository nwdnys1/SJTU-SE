package org.example.backend;

import jakarta.annotation.Resource;
import org.example.backend.repository.MySQLRepository.CartItemRepository;
import org.example.backend.service.CartItemService;
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
