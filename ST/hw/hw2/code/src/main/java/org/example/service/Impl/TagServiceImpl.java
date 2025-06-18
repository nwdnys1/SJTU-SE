package org.example.service.Impl;

import java.util.List;

import org.example.entity.Result;
import org.example.entity.Tag;
import org.example.repository.TagRepository;
import org.example.service.TagService;
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
