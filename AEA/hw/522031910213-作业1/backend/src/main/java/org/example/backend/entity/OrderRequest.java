package org.example.backend.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class OrderRequest {
    private String receiver;
    private String address;
    private String tel;
    private int[] cids;
    private Integer uid;//用户id 用于消息队列
}
