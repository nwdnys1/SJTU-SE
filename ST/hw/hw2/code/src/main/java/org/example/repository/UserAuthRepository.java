package org.example.repository;

import org.example.entity.UserAuth;
import org.springframework.data.jpa.repository.JpaRepository;

public interface UserAuthRepository extends JpaRepository<UserAuth, Integer> {
    UserAuth findUserAuthByUsername(String username);

    void deleteUserAuthById(int id);
    boolean existsUserAuthByUsernameAndPassword(String username, String password);
}
