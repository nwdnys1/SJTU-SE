package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

@Data
@Entity
@Table(name = "replies")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler","comment"})//忽略cartItems属性 并且解决cartItems属性为null的问题
public class Reply {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    @Lob
    private String content;
    private LocalDateTime time;
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "uid")
    @JsonIgnoreProperties({"comments","aboutMe","role","email","tel","enabled","level"})
    private User user;
    @ManyToOne(fetch = FetchType.LAZY)
    @JoinColumn(name = "cid")
    private Comment comment;

}
