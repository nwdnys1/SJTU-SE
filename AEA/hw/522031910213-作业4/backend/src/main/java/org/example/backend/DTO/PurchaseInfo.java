package org.example.backend.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class PurchaseInfo {//某一个用户的每本书的购买信息 用于用户购买信息展示
    private int id;//书籍id
    private String title;
    private int count;//累计购买数量
    private int price;//单价
}
