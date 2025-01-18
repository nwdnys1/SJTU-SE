package org.example.controller;

import java.util.List;

import org.example.entity.Result;
import org.example.entity.Tag;
import org.example.service.TagService;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/tag")
public class TagController {
	TagService service;
	public TagController(TagService service) {
		this.service = service;
	}
	@GetMapping("/list")
	public Result<List<Tag>> list() {
		return service.getTags();
	}
	@GetMapping("/related")
	public Result<List<Long>> related(@RequestParam String name) {
		return service.getRelatedTags(name);
	}
}
