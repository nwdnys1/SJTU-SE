package org.example.backend.DAO;

import org.example.backend.entity.User;
import org.example.backend.entity.UserAuth;

import java.util.List;

public interface UserDAO {
    User getUserByUsername(String username);
    UserAuth findUserAuthByUsername(String username);
    boolean existsUserByUsername(String username);
    boolean existsUserByEmail(String email);
    User getUserById(int id);
    User save(User user, UserAuth userAuth);
    User save(User user);
    boolean existsById(int id);
    void deleteUser(User user);
    List<User> findAll();
}
