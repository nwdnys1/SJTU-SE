package org.example.backend.service.Impl;

import java.util.List;

import org.example.backend.entity.Result;
import org.example.backend.entity.Tag;
import org.example.backend.repository.TagRepository;
import org.example.backend.service.TagService;
import org.springframework.stereotype.Service;

@Service
public class TagServiceImpl implements TagService {
	private final TagRepository repository;
	public TagServiceImpl(TagRepository repository) {
		this.repository = repository;
	}
	public Result<List<Tag>> getTags() {
		return Result.success(repository.allTags());
	}
    public Result<List<Long>> getRelatedTags(String name) {
        return Result.success(repository.findRelatedTags(name));
    }
}
