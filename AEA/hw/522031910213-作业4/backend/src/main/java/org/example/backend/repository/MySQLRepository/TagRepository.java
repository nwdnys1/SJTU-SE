package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.Tag;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

public interface TagRepository extends JpaRepository<Tag, Integer> {

}
