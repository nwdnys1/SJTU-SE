package org.example.backend.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SpentInfo {//每个用户的消费信息 用于消费排行榜
    private int id;
    private String username;
    private int totalSpent;//总消费
}
