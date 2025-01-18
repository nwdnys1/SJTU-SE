package org.example.backend.controller;

import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@CrossOrigin
public class NginxController {
    @RequestMapping("/")
    public String index() {
        return "I'm Service A";
    }
}
