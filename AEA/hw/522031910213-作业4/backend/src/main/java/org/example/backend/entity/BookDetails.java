package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import java.util.List;

@Data
@Entity
@Table(name = "book_details")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler","cartItems","orderItems"})//忽略cartItems属性 并且解决某一字段可能为null的问题
public class BookDetails {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    private int rating;
    private int stock;
    private int sales;//销量
//    @OneToMany(mappedBy = "book",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
//    private List<CartItem> cartItems;
//    @OneToMany(mappedBy = "book", cascade = CascadeType.ALL, orphanRemoval = true, fetch = FetchType.LAZY)
//    private List<OrderItem> orderItems;
//    @JsonIgnoreProperties({"id","content","user","time","replies","likes"})//忽略所有属性 但是要返回comments数组来获得评论数量
//    @OneToMany(mappedBy = "bid",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    @Transient //不映射到数据库
    private List<Comment> comments;
}
