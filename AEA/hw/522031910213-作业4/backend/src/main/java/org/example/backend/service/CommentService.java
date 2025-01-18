package org.example.backend.service;

import org.example.backend.entity.Comment;
import org.example.backend.entity.Reply;
import org.example.backend.entity.Result;
import org.springframework.stereotype.Service;

import java.util.List;
public interface CommentService {
    public Result<List<Comment>> getCommentsByBid(int bid);
    public Result<Comment> addComment(int bid,  String content);
    public Result<Comment> deleteComment(int id);
    public Result<Reply> addReply(int cid, String content);

}
