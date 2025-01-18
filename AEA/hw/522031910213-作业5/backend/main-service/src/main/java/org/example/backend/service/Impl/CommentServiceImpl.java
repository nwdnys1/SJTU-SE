package org.example.backend.service.Impl;

import org.example.backend.DAO.CommentDAO;
import org.example.backend.DAO.UserDAO;
import org.example.backend.entity.*;
import org.example.backend.service.CommentService;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Objects;

@Service
public class CommentServiceImpl implements CommentService {
    private final CommentDAO repository;
    private final UserDAO userDAO;
    public CommentServiceImpl(CommentDAO repository, UserDAO userDAO) {
        this.repository = repository;
        this.userDAO = userDAO;
    }
    public User getUser() {//鉴权获得登录用户
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.getUserByUsername(username);
    }
    public Result<List<Comment>> getCommentsByBid(int bid) {
        List<Comment> comments = repository.getCommentsByBookId(bid);
        return Result.success(comments);
    }
    public Result<Comment> addComment(int bid, String content) {
        User user = getUser();
        Comment comment = new Comment();
        comment.setBid(bid);
        comment.setUser(user);
        comment.setContent(content);
        comment.setTime(LocalDateTime.now());//获取当前时间
        comment.setReplies(List.of());//设置回复为空 防止空指针异常
        comment.setLikes(List.of());//设置点赞为空 防止空指针异常
        repository.save(comment);
        return Result.success(comment);
    }
    public Result<Comment> deleteComment(int id) {
       if (repository.existsById(id)) {
           if(!Objects.equals(repository.getCommentById(id).getUser().getId(), getUser().getId()))
               return Result.error(403,"无权删除");
            repository.deleteById(id);
            return Result.success(null);
        }
        return Result.error(404,"评论不存在");
    }

    public Result<Reply> addReply(int cid, String content) {
        User user = getUser();
        Comment comment = repository.findById(cid);
        if (comment == null) {
            return Result.error(404,"评论不存在");
        }
        Reply reply = new Reply();
        reply.setComment(comment);
        reply.setUser(user);
        reply.setContent(content);
        reply.setTime(LocalDateTime.now());
        comment.getReplies().add(reply);
        repository.save(comment);
        return Result.success(reply);
    }
}
