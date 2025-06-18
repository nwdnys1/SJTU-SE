package org.example.DAO.Impl;

import java.util.List;

import org.example.DAO.CommentDAO;
import org.example.entity.Comment;
import org.example.repository.CommentRepository;
import org.springframework.stereotype.Repository;
@Repository
public class CommentDAOImpl implements CommentDAO {
	private final CommentRepository repository;
	public CommentDAOImpl(CommentRepository repository) {
		this.repository = repository;
	}
	@Override
	public List<Comment> getCommentsByBookId(int bid) {
		return repository.getCommentsByBid(bid);
	}
	@Override
	public Comment getCommentById(String id) {
		return repository.getCommentById(id);
	};
	@Override
	public void save(Comment comment) {
		repository.save(comment);
	}
	@Override
	public boolean existsById(String id) {
		return repository.existsById(id);
	}
	@Override
	public void deleteById(String id) {
		repository.deleteById(id);
	}
	@Override
	public Comment findById(String cid) {
		return repository.findById(cid).orElse(null);
	}
	@Override
	public int countByBookId(int bid) {
		return repository.countByBid(bid);
	}
}
