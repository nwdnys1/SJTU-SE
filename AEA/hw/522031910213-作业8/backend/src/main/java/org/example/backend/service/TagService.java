package org.example.backend.service;

import java.util.List;

import org.example.backend.entity.Result;
import org.example.backend.entity.Tag;
public interface TagService {
	Result<List<Tag>> getTags();

	Result<List<Long>> getRelatedTags(String name);
}
