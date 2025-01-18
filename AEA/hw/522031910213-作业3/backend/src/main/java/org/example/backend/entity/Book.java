package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonIncludeProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.math.BigDecimal;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.List;
import java.util.Map;

@Data
@Entity
@Table(name = "books")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler","cartItems","orderItems"})//忽略cartItems属性 并且解决某一字段可能为null的问题
public class Book {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    private String title;
    private String author;
    @Lob//长文本类型
    private String description;
    @Column(precision = 3,scale = 1)
    private BigDecimal rating;
    private int price;
    private String cover;
    private int stock;
    private String ISBN;
    private int sales;//销量
    @OneToMany(mappedBy = "book",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    private List<CartItem> cartItems;
    @OneToMany(mappedBy = "book", cascade = CascadeType.ALL, orphanRemoval = true, fetch = FetchType.LAZY)
    private List<OrderItem> orderItems;
    @JsonIgnoreProperties({"id","content","user","time","replies","likes"})//忽略所有属性 但是要返回comments数组来获得评论数量
    @OneToMany(mappedBy = "book",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    private List<Comment> comments;
    @ManyToMany(fetch = FetchType.LAZY)
    @JoinTable(name = "book_tag",joinColumns = @JoinColumn(name = "bid"),inverseJoinColumns = @JoinColumn(name = "tid"))
    @JsonIgnoreProperties({"books"})
    private List<Tag> tags;

}
