package org.example.backend.controller;

import org.example.backend.entity.Result;
import org.example.backend.service.LikeService;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/like")
public class LikeController {
    private final LikeService likeService;
    public LikeController(LikeService likeService) {
        this.likeService = likeService;
    }
    @PostMapping("/{cid}")
    public Result<String> likeComment(@PathVariable int cid) {
        return likeService.likeComment(cid);
    }
    @GetMapping("/list")
    public Result<List<Integer>> getCids() {
        return likeService.getCids();
    }
}
