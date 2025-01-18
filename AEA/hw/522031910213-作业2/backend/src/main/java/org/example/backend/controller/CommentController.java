package org.example.backend.controller;

import org.example.backend.entity.Comment;
import org.example.backend.entity.Reply;
import org.example.backend.entity.Result;
import org.example.backend.service.CommentService;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/comment")
public class CommentController {
    private final CommentService service;
    public CommentController(CommentService service) {
        this.service = service;
    }

    @GetMapping("/list/{bid}")
    public Result<List<Comment>> getCommentsByBid(@PathVariable int bid) {
        return service.getCommentsByBid(bid);
    }
    @PostMapping("/add/{bid}")
    public Result<Comment> addComment(@PathVariable int bid,@RequestBody String content) {
        return service.addComment(bid, content);
    }
    @DeleteMapping("/delete/{id}")
    public Result<Comment> deleteComment(@PathVariable int id) {
        return service.deleteComment(id);
    }
    @PostMapping("/reply/{cid}")
    public Result<Reply> addReply(@PathVariable int cid, @RequestBody String content) {
        return service.addReply(cid, content);
    }
}
