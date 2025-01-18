package org.example.backend.config;

import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jdk8.Jdk8Module;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import org.example.backend.component.HibernateCollectionMixIn;
import org.springframework.cache.CacheManager;
import org.springframework.cache.annotation.CachingConfigurer;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.data.redis.cache.RedisCacheConfiguration;
import org.springframework.data.redis.cache.RedisCacheManager;
import org.springframework.data.redis.cache.RedisCacheWriter;
import org.springframework.data.redis.connection.RedisConnectionFactory;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.serializer.GenericJackson2JsonRedisSerializer;
import org.springframework.data.redis.serializer.RedisSerializationContext;
import org.springframework.data.redis.serializer.RedisSerializer;
import org.springframework.data.redis.serializer.StringRedisSerializer;

import java.time.Duration;
import java.util.Collection;
import java.util.Objects;

/**
 * 配置类，用于设置缓存管理器及相关配置，以启用缓存功能
 */
@EnableCaching
@Configuration
public class RedisConfig implements CachingConfigurer {

    /**
     * 配置Redis键的序列化方式
     *
     * @return StringRedisSerializer，用于序列化和反序列化Redis中的键
     */
    private RedisSerializer<String> keySerializer() {
        return new StringRedisSerializer();
    }

    /**
     * 配置Redis值的序列化方式
     *
     * @return GenericJackson2JsonRedisSerializer，使用Jackson库以JSON格式序列化和反序列化Redis中的值
     */
    private RedisSerializer<Object> valueSerializer() {
        ObjectMapper mapper = new ObjectMapper();
        mapper.activateDefaultTyping(mapper.getPolymorphicTypeValidator(), ObjectMapper.DefaultTyping.NON_FINAL, JsonTypeInfo.As.PROPERTY);
        mapper.registerModule(new Jdk8Module());
        mapper.registerModule(new JavaTimeModule());
        mapper.addMixIn(Collection.class, HibernateCollectionMixIn.class);
        return new GenericJackson2JsonRedisSerializer(mapper);
    }

    /**
     * 缓存前缀，用于区分不同的缓存命名空间，一般以模块名或者服务名命名，这里暂时写ebook
     */
    public static final String CACHE_PREFIX = "ebook::";

    /**
     * 配置缓存管理器，使用Redis作为缓存后端
     *
     * @param redisConnectionFactory Redis连接工厂，用于创建Redis连接
     * @return RedisCacheManager，Redis缓存管理器实例
     */
    @Bean
    public CacheManager cacheManager(RedisConnectionFactory redisConnectionFactory) {
        // 配置序列化，解决乱码的问题，设置缓存名称的前缀和缓存条目的默认过期时间
        RedisCacheConfiguration config = RedisCacheConfiguration.defaultCacheConfig()
                // 设置键的序列化器
                .serializeKeysWith(RedisSerializationContext.SerializationPair.fromSerializer(keySerializer()))
                // 设置值的序列化器
                .serializeValuesWith(RedisSerializationContext.SerializationPair.fromSerializer(valueSerializer()))
                // 设置缓存名称的前缀
                .computePrefixWith(name -> CACHE_PREFIX + name + ":")
                // 设置缓存条目的默认过期时间为300秒
                .entryTtl(Duration.ofSeconds(300));

        // 创建非锁定的Redis缓存写入器
        RedisCacheWriter redisCacheWriter = RedisCacheWriter.nonLockingRedisCacheWriter(Objects.requireNonNull(redisConnectionFactory));

        // 返回Redis缓存管理器实例，使用上述配置
        return new RedisCacheManager(redisCacheWriter, config);
    }

    @Bean
    public RedisTemplate<String, Object> redisTemplate(RedisConnectionFactory redisConnectionFactory) {
        RedisTemplate<String, Object> template = new RedisTemplate<>();
        template.setConnectionFactory(redisConnectionFactory);//设置连接工厂
        template.setKeySerializer(keySerializer());
        template.setValueSerializer(valueSerializer());
        // You can also configure other serializers as needed
        return template;
    }

}

