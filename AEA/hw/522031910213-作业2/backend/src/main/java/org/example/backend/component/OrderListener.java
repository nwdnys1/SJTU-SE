package org.example.backend.component;

import com.alibaba.fastjson2.JSON;
import com.alibaba.fastjson2.TypeReference;
import org.example.backend.entity.Order;
import org.example.backend.entity.OrderRequest;
import org.example.backend.entity.Result;
import org.example.backend.service.OrderService;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.stereotype.Component;

import java.io.IOException;

@Component
public class OrderListener {
    private final OrderService service;
    private final KafkaTemplate<String, String> kafkaTemplate;
    private final OrderWS orderWS;
    public OrderListener(OrderService service, KafkaTemplate<String, String> kafkaTemplate, OrderWS orderWS) {
        this.service = service;
        this.kafkaTemplate = kafkaTemplate;
        this.orderWS = orderWS;
    }
    //监听订单topic
    @KafkaListener(topics = "AddOrder",groupId = "AddOrder",concurrency = "1")//3个消费者并发处理
    public void addOrder(String newOrder) {
        System.out.println("Received Order: " + newOrder);
        OrderRequest order = JSON.parseObject(newOrder, OrderRequest.class);
        try{
            kafkaTemplate.send("FinishOrder", service.addOrder(order).asJsonString());
        }catch (Exception e){
            System.out.println("Failed to add order: " + e.getMessage());
        }
    }
    //监听订单完成topic
    @KafkaListener(topics = "FinishOrder",groupId = "FinishOrder",concurrency = "1")//3个消费者并发处理
    public void finishOrder(String newOrder)  {
        Result<Order> result = JSON.parseObject(newOrder, new TypeReference<Result<Order>>() {});
        if (result.getCode() == 200) {
            orderWS.SendToUser(result.getData().getUid(), "您的订单已成功提交！");
        }
        else {
            orderWS.SendToUser(result.getData().getUid(), "订单处理异常："+result.getMessage());
        }
    }

}
