package org.example.repository;

import java.util.List;

import org.example.entity.BookTag;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

public interface BookTagRepository extends JpaRepository<BookTag, Integer> {
	@Query("SELECT DISTINCT bt.tid FROM BookTag bt WHERE bt.bid = :bid")
	List<Long> findTidsByBid(@Param("bid") int bid);
	@Query("SELECT DISTINCT b.bid FROM BookTag b WHERE b.tid IN :tids") // 查询所有与指定 tid 相关的 bid（分页查询）DISTINCT 去重
	Page<Integer> findBidsByTids(@Param("tids") List<Long> tids, Pageable pageable);
}
