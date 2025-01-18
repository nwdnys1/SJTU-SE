package org.example.backend.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class OrderRequest {
    private String receiver;
    private String address;
    private String tel;
    private List<Integer> cids;//购物车id
    private Integer uid;//用户id 用于消息队列
}
