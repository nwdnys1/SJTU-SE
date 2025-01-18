package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.Comment;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

public interface CommentRepository extends JpaRepository<Comment, Integer> {
    List<Comment> getCommentsByBookId(int bid);
    Comment getCommentById(int id);
}
