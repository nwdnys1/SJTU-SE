package org.example.DAO;

import org.example.entity.Comment;

import java.util.List;

public interface CommentDAO {
    List<Comment> getCommentsByBookId(int bid);

    Comment getCommentById(String id);

    void save(Comment comment);

    boolean existsById(String id);

    void deleteById(String id);

    Comment findById(String cid);

    int countByBookId(int bid);
}
