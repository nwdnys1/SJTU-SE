package org.example.backend.service.Impl;

import org.example.backend.DAO.UserDAO;
import org.example.backend.entity.Comment;
import org.example.backend.entity.Like;
import org.example.backend.entity.Result;
import org.example.backend.entity.User;
import org.example.backend.repository.LikeRepository;
import org.example.backend.service.LikeService;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

@Service
public class LikeServiceImpl implements LikeService {
    private final LikeRepository repository;
    private final UserDAO userDAO;
    public LikeServiceImpl(LikeRepository repository, UserDAO userDAO) {
        this.repository = repository;
        this.userDAO = userDAO;
    }
    public User getUser() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.getUserByUsername(username);
    }
    public Result<String> likeComment(int cid) {
        User user = getUser();
        Like like = repository.getLikeByUserIdAndCommentId(user.getId(), cid);
        if(like != null)
        {
            repository.delete(like);
            return Result.success("取消点赞");
        }
        like = new Like();
        like.setComment(new Comment());
        like.getComment().setId(cid);
        like.setUser(user);
        repository.save(like);
        return Result.success("点赞成功");
    }
    public Result<List<Integer>> getCids() {
        int uid = getUser().getId();
        List<Integer> cids = new ArrayList<>();
        List<Like> likes = repository.getLikesByUserId(uid);
        for(Like like : likes)
        {
            cids.add(like.getComment().getId());
        }
        return Result.success(cids);
    }
}
