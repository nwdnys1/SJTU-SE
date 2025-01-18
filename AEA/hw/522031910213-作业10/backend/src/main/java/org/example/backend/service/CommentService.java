package org.example.backend.service;

import java.util.List;

import org.example.backend.entity.Comment;
import org.example.backend.entity.Reply;
import org.example.backend.entity.Result;
public interface CommentService {
	public Result<List<Comment>> getCommentsByBid(int bid);
	public Result<Comment> addComment(Comment comment);
	public Result<Comment> deleteComment(String id);
	public Result<Reply> addReply(String cid, String content);
	public Result<String> likeComment(String cid);

}
