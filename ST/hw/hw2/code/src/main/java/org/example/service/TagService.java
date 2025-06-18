package org.example.service;

import java.util.List;

import org.example.entity.Result;
import org.example.entity.Tag;

public interface TagService {
	Result<List<Tag>> getTags();

	Result<List<Long>> getRelatedTags(String name);
}
