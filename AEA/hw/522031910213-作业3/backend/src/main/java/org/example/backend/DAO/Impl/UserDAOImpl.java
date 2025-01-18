package org.example.backend.DAO.Impl;

import org.example.backend.DAO.UserDAO;
import org.example.backend.entity.User;
import org.example.backend.entity.UserAuth;
import org.example.backend.repository.MySQLRepository.MysqlUserRepository;
import org.example.backend.repository.MySQLRepository.UserAuthRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public class UserDAOImpl implements UserDAO {
    private final MysqlUserRepository mysqlUserRepository;
    private final UserAuthRepository userAuthRepository;
    public UserDAOImpl(MysqlUserRepository mysqlUserRepository, UserAuthRepository userAuthRepository) {
        this.mysqlUserRepository = mysqlUserRepository;
        this.userAuthRepository = userAuthRepository;
    }

    @Override
    public User findUserByUsername(String username) {
        return mysqlUserRepository.findUserByUsername(username);
    }
    @Override
    public UserAuth findUserAuthByUsername(String username) {
        return userAuthRepository.findUserAuthByUsername(username);
    }
    @Override
    public boolean existsUserByUsername(String username) {
        return mysqlUserRepository.existsUserByUsername(username);
    }

    @Override
    public boolean existsUserByEmail(String email) {
        return mysqlUserRepository.existsUserByEmail(email);
    }

    @Override
    public User findById(int id) {
        return mysqlUserRepository.findById(id).orElse(null);
    }

    @Override
    public void save(User user,UserAuth userAuth) {//这个方法用于注册用户
        mysqlUserRepository.save(user);
        userAuthRepository.save(userAuth);
    }
    @Override
    public void save(User user) {//这个方法只更新用户信息，不更新密码
        mysqlUserRepository.save(user);
    }
    @Override
    public boolean existsById(int id) {
        return mysqlUserRepository.existsById(id);
    }

    @Override
    public void deleteById(int id) {
        mysqlUserRepository.deleteById(id);
        userAuthRepository.deleteUserAuthById(id);
    }

    @Override
    public List<User> findAll() {
        return mysqlUserRepository.findAll();
    }
}