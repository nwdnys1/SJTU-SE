package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.Like;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;


public interface LikeRepository extends JpaRepository<Like, Integer> {
    public Like getLikeByUserIdAndCommentId(int uid, int cid);
    public List<Like> getLikesByUserId(int uid);

}
