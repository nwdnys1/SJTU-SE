package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

@Data
@Entity
@Table(name = "books")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler"})//忽略cartItems属性 并且解决某一字段可能为null的问题
public class Book {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    private String title;
    private String author;
    @Lob//长文本类型
    private String description;
    private int price;
    private String cover;
    private String isbn;
    @ManyToMany(fetch = FetchType.LAZY)
    @JoinTable(name = "book_tag",joinColumns = @JoinColumn(name = "bid"),inverseJoinColumns = @JoinColumn(name = "tid"))
    @JsonIgnoreProperties({"books"})
    private List<Tag> tags;
    @OneToOne(cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    @JoinColumn(name = "id")
    private BookDetails bookDetails;

}
