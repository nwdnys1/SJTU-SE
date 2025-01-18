package org.example.backend.DAO;

import org.example.backend.entity.Comment;

import java.util.List;

public interface CommentDAO {
    List<Comment> getCommentsByBookId(int bid);

    Comment getCommentById(int id);

    void save(Comment comment);

    boolean existsById(int id);

    void deleteById(int id);

    Comment findById(int cid);
}
