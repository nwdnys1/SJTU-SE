package org.example.backend.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SalesInfo {//每本书的销量信息 用于销量排行榜
    private int id;
    private String title;
    private int sales;//销量
}
