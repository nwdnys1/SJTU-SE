package org.example.utils;

import lombok.Setter;
import org.springframework.context.annotation.Scope;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;
import java.util.concurrent.TimeUnit;

/**
 * redisTemplate封装
 *
 *  @author david
 */
@Component
@Scope("prototype")//多例 每一个DAO层实例对应一个RedisUtil实例
public class RedisUtil {

    private final RedisTemplate<String, Object> redisTemplate;
    @Setter
    private String PREFIX;
    public RedisUtil(RedisTemplate<String, Object> redisTemplate) {
        this.redisTemplate = redisTemplate;
    }
    /**
     * 普通缓存获取
     * @param key 键
     * @return 值
     */
    public Object get(String key){
        key=PREFIX+key;
        return redisTemplate.opsForValue().get(key);
    }

    /**
     * 普通缓存放入
     * @param key 键
     * @param value 值
     * @return true成功 false失败
     */
    public void set(String key,Object value) {
        key=PREFIX+key;
        try {
            redisTemplate.opsForValue().set(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void del(String key) {
        key=PREFIX+key;
        redisTemplate.delete(key);
    }


}