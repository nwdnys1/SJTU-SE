package org.example.backend.service;

import org.example.backend.entity.Like;
import org.example.backend.entity.Result;
import org.springframework.stereotype.Service;

import java.util.List;

public interface LikeService {

    Result<String> likeComment(int cid);
    Result<List<Integer>> getCids();
}
