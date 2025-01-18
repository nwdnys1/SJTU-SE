package org.example.backend.controller;

import java.util.List;

import org.example.backend.entity.Comment;
import org.example.backend.entity.Reply;
import org.example.backend.entity.Result;
import org.example.backend.service.CommentService;
import org.springframework.web.bind.annotation.*;

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
	@PostMapping("/add")
	public Result<Comment> addComment( @RequestBody Comment comment) {
		return service.addComment( comment);
	}
	@DeleteMapping("/delete/{id}")
	public Result<Comment> deleteComment(@PathVariable String id) {
		return service.deleteComment(id);
	}
	@PostMapping("/reply/{cid}")
	public Result<Reply> addReply(@PathVariable String cid, @RequestBody String content) {
		return service.addReply(cid, content);
	}
	@PostMapping("like/{cid}")
	public Result<String> likeComment(@PathVariable String cid) {
		return service.likeComment(cid);
	}
}
