package org.example.DAO;

import org.example.entity.User;
import org.example.entity.UserAuth;

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
