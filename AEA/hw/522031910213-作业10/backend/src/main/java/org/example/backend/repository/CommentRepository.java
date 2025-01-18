package org.example.backend.repository;

import org.example.backend.entity.Comment;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;

public interface CommentRepository extends MongoRepository<Comment, String> {
    List<Comment> getCommentsByBid(int id);
    Comment getCommentById(String id);
    int countByBid(int id);//统计评论数
}
