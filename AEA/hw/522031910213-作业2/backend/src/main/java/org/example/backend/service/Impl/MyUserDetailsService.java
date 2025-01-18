package org.example.backend.service.Impl;

import org.example.backend.DAO.OrderDAO;
import org.example.backend.DAO.UserDAO;
import org.example.backend.DTO.PurchaseInfo;
import org.example.backend.DTO.SalesInfo;
import org.example.backend.DTO.SpentInfo;
import org.example.backend.entity.*;
import org.example.backend.repository.MySQLRepository.UploadRepository;
import org.springframework.security.authentication.DisabledException;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

@Service
public class MyUserDetailsService implements UserDetailsService {
    UserDAO userDAO;
    UploadRepository uploadRepository;
    OrderDAO orderDAO;
    public MyUserDetailsService(UserDAO userDAO, UploadRepository uploadRepository, OrderDAO orderDAO) {
        this.userDAO = userDAO;
        this.uploadRepository = uploadRepository;
        this.orderDAO = orderDAO;
    }
    @Override
    public UserDetails loadUserByUsername(String username) throws UsernameNotFoundException, DisabledException{
        User user = userDAO.findUserByUsername(username);
        if (user == null) {
            throw new UsernameNotFoundException("未找到用户");
        }
        String pwd = userDAO.findUserAuthByUsername(username).getPassword();
        List< GrantedAuthority > authorities = new ArrayList<>();
        authorities.add((GrantedAuthority) () -> "ROLE_" + user.getRole());
        System.out.println("loadUserByUsername: "+user.getUsername());
        return new org.springframework.security.core.userdetails.User(user.getUsername(), pwd, user.isEnabled(), true, true, true, authorities);
    }
    public Result<User> updateUser(UserProfile request) {
        int id = getUid();
        User user = userDAO.findById(id);
        if (user == null) {
            return Result.error(404, "用户不存在！");
        }
        if(userDAO.existsUserByUsername(request.getUsername()) && !request.getUsername().equals(user.getUsername())) {
            return Result.error(400, "用户名已存在！");
        }
        user.setUsername(request.getUsername());
        user.setEmail(request.getEmail());
        user.setTel(request.getTel());
        user.setAboutMe(request.getAboutMe());
        userDAO.save(user);
        return Result.success(user);

    }
    public Result<User> addUser(RegisterRequest request) {
        if(userDAO.existsUserByUsername(request.getUsername())) {
            return Result.error(400, "用户名已存在！");
        }
        if(userDAO.existsUserByEmail(request.getEmail())) {
            return Result.error(400, "邮箱已被占用！");
        }
        User user = new User();
        user.setUsername(request.getUsername());
        user.setEmail(request.getEmail());
        user.setRole("user");
        user.setEnabled(true);
        user.setLevel(1);
        user.setAvatar("https://img.moegirl.org.cn/common/b/b7/Transparent_Akkarin.jpg");//默认头像
        user.setTel("");
        user.setAboutMe("");
        UserAuth userAuth = new UserAuth();
        userAuth.setUsername(request.getUsername());
        userAuth.setPassword(new BCryptPasswordEncoder().encode(request.getPassword()));
        userDAO.save(user, userAuth);
        return Result.success(user);
    }
    public Result<User> deleteUser(int id) {
        if (userDAO.existsById(id)) {
            userDAO.deleteById(id);
            return Result.success(null);
        } else {
            return Result.error(404, "用户不存在！");
        }
    }
    public Result<User> getUserByUsername(String username) {
        User user = userDAO.findUserByUsername(username);
        if (user == null) {
            return Result.error(404, "用户不存在！");
        }
        return Result.success(user);
    }
    public int getUid() {//从数据库里查询id
        String username = ((UserDetails) SecurityContextHolder.getContext().getAuthentication().getPrincipal()).getUsername();
        return userDAO.findUserByUsername(username).getId();
    }
    public Result<String> updateAvatar(MultipartFile file)  {
        int id = getUid();
        User user = userDAO.findById(id);
        if (user == null) {
            return Result.error(404, "用户不存在！");
        }
        try {
            String url = uploadRepository.uploadFile(file, "image");
            user.setAvatar(url);
            userDAO.save(user);
            return Result.success(url);
        }
        catch (IOException e) {
            return Result.error(500, e.getMessage());
        }
    }

    public Result<User> disableUser(int id) {
        User user = userDAO.findById(id);
        if (user == null) {
            return Result.error(404, "用户不存在！");
        }
        if(user.getRole().equals("admin")) {
            return Result.error(403, "无法禁用管理员！");
        }
        user.setEnabled(false);
        userDAO.save(user);
        return Result.success(user);
    }

    public Result<List<User>> getAllUsers() {
        return Result.success(userDAO.findAll());
    }

    public Result<User> enableUser(int id) {
        User user = userDAO.findById(id);
        if (user == null) {
            return Result.error(404, "用户不存在！");
        }
        user.setEnabled(true);
        userDAO.save(user);
        return Result.success(user);
    }

    public Result<List<SpentInfo>> rank(LocalDateTime start, LocalDateTime end, int nums) {
        LinkedHashMap<Integer, SpentInfo> spentInfos = new LinkedHashMap<>();
        List<Order> orders= orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end);//找到所有时间段内的订单
        for(Order order: orders){
            Integer uid = order.getUser().getId();
            List<OrderItem> orderItems = order.getOrderItems();
            int totalSpent = 0;
            for(OrderItem orderItem: orderItems){
               totalSpent += orderItem.getBook().getPrice() * orderItem.getQuantity();
            }
            if(spentInfos.containsKey(uid)) {
                spentInfos.get(uid).setTotalSpent(spentInfos.get(uid).getTotalSpent() + totalSpent);
            } else{
                spentInfos.put(uid, new SpentInfo(uid,order.getUser().getUsername(), totalSpent));
            }
        }
        List<SpentInfo> list = new ArrayList<>(spentInfos.values());
        list.sort((a, b) -> b.getTotalSpent() - a.getTotalSpent());//按消费金额降序排列
        return Result.success(list.subList(0, Math.min(nums, list.size())));
    }

    public Result<List<PurchaseInfo>> statistics(LocalDateTime start, LocalDateTime end) {
        LinkedHashMap<Integer, PurchaseInfo> purchaseInfos = new LinkedHashMap<>();
        List<Order> orders= orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBeforeAndUserId(start, end,getUid());//找到所有时间段内的订单
        for(Order order: orders){
            List<OrderItem> orderItems = order.getOrderItems();
            for(OrderItem orderItem: orderItems){
                Integer bid = orderItem.getBook().getId();
                if(purchaseInfos.containsKey(bid)){
                    purchaseInfos.get(bid).setCount(purchaseInfos.get(bid).getCount() + orderItem.getQuantity());
                }else{
                    purchaseInfos.put(bid, new PurchaseInfo(bid, orderItem.getBook().getTitle(), orderItem.getQuantity(), orderItem.getBook().getPrice()));
                }
            }
        }
        List<PurchaseInfo> list = new ArrayList<>(purchaseInfos.values());
        list.sort((a, b) -> b.getCount() - a.getCount());//按消费金额降序排列
        return Result.success(list);
    }
}
