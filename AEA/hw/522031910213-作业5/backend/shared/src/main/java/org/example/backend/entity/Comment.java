package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;
import java.util.List;

@Data
@Entity
@Table(name = "comments")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler","bookDetails"})//忽略book属性，并解决replies属性为空的问题
public class Comment {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    @Lob
    @Column(columnDefinition = "TEXT")
    private String content;
    private LocalDateTime time;
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "uid")
    @JsonIgnoreProperties({"comments","aboutMe","role","email","tel","enabled","level"})
    private User user;
//    @ManyToOne(fetch = FetchType.LAZY)
//    @JoinColumn(name = "bid")
//    private BookDetails bookDetails;
    @JoinColumn(name = "bid")
    private Integer bid;
    @OneToMany(mappedBy = "comment",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    private List<Reply> replies;
    @OneToMany(mappedBy = "comment",cascade = CascadeType.ALL,orphanRemoval = true,fetch = FetchType.LAZY)
    @JsonIgnoreProperties({"comment","id"})
    private List<Like> likes;
}
