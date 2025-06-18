//package org.example.backend.component;
//
//import org.example.backend.entity.User;
//import org.example.backend.repository.MySQLRepository.UserAuthRepository;
//import org.springframework.security.authentication.BadCredentialsException;
//import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
//import org.springframework.security.authentication.AuthenticationProvider;
//import org.springframework.security.core.Authentication;
//import org.springframework.security.core.AuthenticationException;
//import org.springframework.security.core.GrantedAuthority;
//import org.springframework.security.crypto.password.PasswordEncoder;
//import org.springframework.stereotype.Component;
//
//import java.io.PrintWriter;
//import java.util.ArrayList;
//import java.util.List;
//@Component
//public class MyAuthenticationProvider implements AuthenticationProvider {
//    private final UserAuthRepository repository;
//    private final PasswordEncoder passwordEncoder;
//
//    public MyAuthenticationProvider(UserAuthRepository repository, PasswordEncoder passwordEncoder) {
//        this.repository = repository;
//        this.passwordEncoder = passwordEncoder;
//    }
//    @Override
//    public Authentication authenticate(Authentication authentication) throws AuthenticationException {
//        String username = authentication.getName();
//        String password = authentication.getCredentials().toString();
//        System.out.println("myauthprovider: "+username+" "+password);
//        // 根据用户名和密码查询数据库，判断是否存在对应记录
//        if (passwordEncoder.matches(password,repository.findUserAuthByUsername(username).getPassword())) {//matches方法用于比较明文和加密后的密码是否相等
//            // 如果存在，返回一个完全认证的Authentication对象
//            User user=repository.findUserAuthByUsername(username).getUser();
//            List<GrantedAuthority> authorities = new ArrayList<>();
//            authorities.add((GrantedAuthority) () -> "ROLE_" + user.getRole());
//            return new UsernamePasswordAuthenticationToken(username, password, authorities);
//        } else {
//            throw new BadCredentialsException("密码错误");
//        }
//    }
//
//    @Override
//    public boolean supports(Class<?> authentication) {
//        return authentication.equals(UsernamePasswordAuthenticationToken.class);
//    }
//}
