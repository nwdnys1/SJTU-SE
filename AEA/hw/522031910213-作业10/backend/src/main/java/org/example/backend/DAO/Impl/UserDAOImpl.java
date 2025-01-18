package org.example.backend.DAO.Impl;

import java.util.List;

import org.example.backend.DAO.UserDAO;
import org.example.backend.entity.User;
import org.example.backend.entity.UserAuth;
import org.example.backend.repository.UserAuthRepository;
import org.example.backend.repository.UserRepository;
import org.example.backend.utils.RedisUtil;
import org.springframework.stereotype.Repository;

@Repository
public class UserDAOImpl implements UserDAO {
	private final UserRepository repository;
	private final UserAuthRepository userAuthRepository;
	private final RedisUtil redisUtil;
	public UserDAOImpl(UserRepository repository, UserAuthRepository userAuthRepository, RedisUtil redisUtil) {
		this.repository = repository;
		this.userAuthRepository = userAuthRepository;
		this.redisUtil = redisUtil;
		redisUtil.setPREFIX("user::");
	}

	@Override
	public User getUserByUsername(String username) {
		try {
			User user = (User) redisUtil.get("username::" + username);
			if (user == null) {
				user = repository.findUserByUsername(username);
				redisUtil.set("username::" + username, user);
			}
			return user;
		} catch (Exception e) { // redis error or other error return from MySQL
			System.out.println("Redis error: " + e.getMessage());
			return repository.findUserByUsername(username);
		}
	}
	@Override
	public User getUserById(int id) {
		try {
			User user = (User) redisUtil.get("id::" + id);
			if (user == null) {
				user = repository.findById(id).orElse(null);
				redisUtil.set("id::" + id, user);
			}
			return user;
		} catch (Exception e) { // redis error or other error return from MySQL
			System.out.println("Redis error: " + e.getMessage());
			return repository.findById(id).orElse(null);
		}
	}
	@Override
	public UserAuth findUserAuthByUsername(String username) {
		return userAuthRepository.findUserAuthByUsername(username);
	}
	@Override
	public boolean existsUserByUsername(String username) {
		return repository.existsUserByUsername(username);
	}

	@Override
	public boolean existsUserByEmail(String email) {
		return repository.existsUserByEmail(email);
	}

	@Override
	public User save(User user, UserAuth userAuth) {// 这个方法用于注册用户
		user = repository.save(user);
		userAuthRepository.save(userAuth);
		redisUtil.set("id::" + user.getId(), user);
		redisUtil.set("username::" + user.getUsername(), user);
		System.out.println("Save user to redis");
		return user;
	}
	@Override
	public User save(User user) {// 这个方法只更新用户信息，不更新密码
		user = repository.save(user);
		redisUtil.set("id::" + user.getId(), user);
		redisUtil.set("username::" + user.getUsername(), user);
		System.out.println("Save user to redis");
		repository.save(user);
		return user;
	}
	@Override
	public boolean existsById(int id) {
		return repository.existsById(id);
	}

	@Override
	public void deleteUser(User user) {
		redisUtil.del("id::" + user.getId());
		redisUtil.del("username::" + user.getUsername());
		System.out.println("Delete user from redis");
		repository.deleteById(user.getId());
		userAuthRepository.deleteUserAuthById(user.getId());
	}

	@Override
	public List<User> findAll() {
		return repository.findAll();
	}
}