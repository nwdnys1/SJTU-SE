package org.example.backend;

import java.util.function.Function;

import org.example.backend.entity.OrderItem;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration;
import org.springframework.context.annotation.Bean;

import com.alibaba.fastjson2.JSONObject;

import org.springframework.web.bind.annotation.CrossOrigin;
import reactor.core.publisher.Flux;

@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
public class BackendApplication {

	public static void main(String[] args) {
		SpringApplication.run(BackendApplication.class, args);

	}

	@Bean
	public Function<Flux<String>, Flux<Integer>> computePrice() {
		return flux -> flux.map(value -> {
			OrderItem orderItem = JSONObject.parseObject(value, OrderItem.class);
			return orderItem.getBook().getPrice() * orderItem.getQuantity();
		}).reduce(0, Integer::sum).flux();
	}

}
