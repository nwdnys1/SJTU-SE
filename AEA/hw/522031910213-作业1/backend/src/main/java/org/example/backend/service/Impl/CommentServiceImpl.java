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
@Service
public class CommentServiceImpl implements CommentService {
    private final CommentDAO repository;
    private final UserDAO userDAO;
    public CommentServiceImpl(CommentDAO repository, UserDAO userDAO) {
        this.repository = repository;
        this.userDAO = userDAO;
    }
    public int getUid() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.findUserByUsername(username).getId();
    }
    public Result<List<Comment>> getCommentsByBid(int bid) {
        List<Comment> comments = repository.getCommentsByBookId(bid);
        return Result.success(comments);
    }
    public Result<Comment> addComment(int bid, String content) {
        int uid = getUid();
        Comment comment = new Comment();
        comment.setBook(new Book());
        comment.getBook().setId(bid);
        User user = userDAO.findById(uid);
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
           if(repository.getCommentById(id).getUser().getId() != getUid())
               return Result.error(403,"无权删除");
            repository.deleteById(id);
            return Result.success(null);
        }
        return Result.error(404,"评论不存在");
    }

    public Result<Reply> addReply(int cid, String content) {
        int uid = getUid();
        Comment comment = repository.findById(cid);
        if (comment == null) {
            return Result.error(404,"评论不存在");
        }
        Reply reply = new Reply();
        reply.setComment(comment);
        User user = userDAO.findById(uid);
        if (user == null) {
            return Result.error(404,"用户不存在");
        }
        reply.setUser(user);
        reply.setContent(content);
        reply.setTime(LocalDateTime.now());
        comment.getReplies().add(reply);
        repository.save(comment);
        return Result.success(reply);
    }
}
