package org.example.backend.repository.MySQLRepository;

import org.example.backend.entity.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

public interface MysqlUserRepository extends JpaRepository<User, Integer> {
    User findUserByUsername(String username);
    boolean existsUserByUsername(String username);
    boolean existsUserByEmail(String email);


}