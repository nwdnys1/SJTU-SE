package org.example.backend.config;

import org.apache.kafka.clients.admin.NewTopic;
import org.springframework.context.annotation.Bean;

public class KafkaConfig {
    @Bean
    public NewTopic AddOrder() {//订单topic
        return new NewTopic("AddOrder", 1, (short) 1);
    }
    @Bean
    public NewTopic FinishOrder() {//订单完成topic
        return new NewTopic("FinishOrder", 1, (short) 1);
    }

}
