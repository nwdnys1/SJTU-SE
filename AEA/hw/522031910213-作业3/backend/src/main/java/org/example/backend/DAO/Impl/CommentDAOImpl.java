package org.example.backend.DAO.Impl;

import org.example.backend.DAO.CommentDAO;
import org.example.backend.entity.Comment;
import org.example.backend.repository.MySQLRepository.CommentRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
@Repository
public class CommentDAOImpl implements CommentDAO {
    private final CommentRepository repository;
    public CommentDAOImpl(CommentRepository repository) {
        this.repository = repository;
    }
    @Override
    public List<Comment> getCommentsByBookId(int bid){
        return repository.getCommentsByBookId(bid);
    };
    @Override
    public Comment getCommentById(int id){
        return repository.getCommentById(id);
    };

    @Override
    public void save(Comment comment) {
        repository.save(comment);
    }

    @Override
    public boolean existsById(int id) {
        return repository.existsById(id);
    }

    @Override
    public void deleteById(int id) {
        repository.deleteById(id);
    }

    @Override
    public Comment findById(int cid) {
        return repository.findById(cid).orElse(null);
    }
}
