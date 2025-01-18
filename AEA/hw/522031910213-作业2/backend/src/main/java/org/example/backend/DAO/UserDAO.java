package org.example.backend.DAO;

import org.example.backend.entity.User;
import org.example.backend.entity.UserAuth;

import java.util.List;

public interface UserDAO {
    User findUserByUsername(String username);
    UserAuth findUserAuthByUsername(String username);
    boolean existsUserByUsername(String username);
    boolean existsUserByEmail(String email);
    User findById(int id);
    void save(User user, UserAuth userAuth);
    void save(User user);
    boolean existsById(int id);
    void deleteById(int id);
    List<User> findAll();
}
