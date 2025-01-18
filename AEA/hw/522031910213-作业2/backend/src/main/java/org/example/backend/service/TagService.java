package org.example.backend.service;

import org.example.backend.entity.Result;
import org.example.backend.entity.Tag;
import org.springframework.stereotype.Service;

import java.util.List;
public interface TagService {
    public Result<List<Tag>> getTags();

}
