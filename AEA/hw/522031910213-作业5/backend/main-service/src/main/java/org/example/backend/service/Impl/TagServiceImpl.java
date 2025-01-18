package org.example.backend.service.Impl;

import org.example.backend.entity.Result;
import org.example.backend.entity.Tag;
import org.example.backend.repository.TagRepository;
import org.example.backend.service.TagService;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class TagServiceImpl implements TagService {
    private final TagRepository repository;
    public TagServiceImpl(TagRepository repository) {
        this.repository = repository;
    }
    public Result<List<Tag>> getTags() {
        return Result.success(repository.findAll());
    }
}
