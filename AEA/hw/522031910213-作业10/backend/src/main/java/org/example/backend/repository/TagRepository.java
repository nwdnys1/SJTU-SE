package org.example.backend.repository;

import java.util.List;

import org.example.backend.entity.Tag;
import org.springframework.data.neo4j.repository.Neo4jRepository;
import org.springframework.data.neo4j.repository.query.Query;
public interface TagRepository extends Neo4jRepository<Tag, Long> {
	@Query("MATCH (t:Tag) WHERE ID(t) IN $0 RETURN t")
	List<Tag> findTagNamesByTids(List<Long> tids);
	@Query("MATCH (t:Tag) RETURN t")
	List<Tag> allTags();
	@Query("MATCH (t1:Tag)-[:RELATE_TO]->(t2)-[:RELATE_TO]->(t3) WHERE t1.name = $0 WITH COLLECT(t1) + COLLECT(t2) + COLLECT(t3) AS t4 UNWIND t4 AS t RETURN DISTINCT ID(t)")
	List<Long> findRelatedTags(String name);
}
