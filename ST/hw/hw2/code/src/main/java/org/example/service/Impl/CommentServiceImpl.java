package org.example.service.Impl;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Objects;

import org.example.DAO.CommentDAO;
import org.example.DAO.UserDAO;
import org.example.entity.Comment;
import org.example.entity.Reply;
import org.example.entity.Result;
import org.example.entity.User;
import org.example.service.CommentService;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.data.mongodb.core.query.Update;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;

import com.mongodb.client.result.UpdateResult;

@Service
public class CommentServiceImpl implements CommentService {
	private final CommentDAO repository;
	private final UserDAO userDAO;
	private final MongoTemplate mongoTemplate;
	public CommentServiceImpl(CommentDAO repository, UserDAO userDAO, MongoTemplate mongoTemplate) {
		this.repository = repository;
		this.userDAO = userDAO;
		this.mongoTemplate = mongoTemplate;
	}
	public User getUser() {// 鉴权获得登录用户
		String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal())
				.getUsername();
		return userDAO.getUserByUsername(username);
	}
	public Result<List<Comment>> getCommentsByBid(int bid) {
		List<Comment> comments = repository.getCommentsByBookId(bid);
		return Result.success(comments);
	}
	public Result<Comment> addComment(Comment comment) {
		comment.setUid(getUser().getId());
		comment.setTime(LocalDateTime.now());// 获取当前时间
		comment.setReplies(List.of());// 设置回复为空 防止空指针异常
		comment.setLikes(List.of());// 设置点赞为空 防止空指针异常
		repository.save(comment);
		return Result.success(comment);
	}
	public Result<Comment> deleteComment(String id) {
		if (repository.existsById(id)) {
			if (!Objects.equals(repository.getCommentById(id).getUid(), getUser().getId()))
				return Result.error(403, "无权删除");
			repository.deleteById(id);
			return Result.success(null);
		}
		return Result.error(404, "评论不存在");
	}

	public Result<Reply> addReply(String cid, String content) {
		Query query = new Query(Criteria.where("id").is(cid));
		Update update = new Update().push("replies", new Reply(content, LocalDateTime.now(), getUser().getId()));
		// 执行更新操作，不需要加载整个评论对象
		UpdateResult result = mongoTemplate.updateFirst(query, update, Comment.class);
		if (result.getMatchedCount() == 0) {
			return Result.error(404, "评论不存在");
		}
		return Result.success(new Reply(content, LocalDateTime.now(), getUser().getId()));
	}

	public Result<String> likeComment(String cid) {
		// 创建查询条件：根据评论 ID 查找
		Query query = new Query(Criteria.where("id").is(cid));
		// 只加载 'likes' 列表字段
		query.fields().include("likes");
		// 获取当前评论的点赞列表
		Comment comment = mongoTemplate.findOne(query, Comment.class);
		// 如果评论不存在，返回 null 或者可以抛出异常
		if (comment == null) {
			return Result.error(404, "评论不存在");
		}
		List<Integer> likes = comment.getLikes(); // 获取点赞列表
		// 检查用户是否已经点赞
		int uid = getUser().getId();
		Update update;
		boolean liked = likes.contains(uid);
		if (liked) {
			// 如果已点赞，移除点赞
			update = new Update().pull("likes", uid);
		} else {
			// 如果未点赞，添加点赞
			update = new Update().addToSet("likes", uid);
		}
		mongoTemplate.updateFirst(query, update, Comment.class);
		// 返回更新后的点赞列表
		return liked ? Result.success("取消点赞") : Result.success("点赞成功");
	}
}
