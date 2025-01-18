package org.example.backend.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.List;

@Data
@Entity
@Table(name = "tags")
@AllArgsConstructor
@NoArgsConstructor
@JsonIgnoreProperties({"hibernateLazyInitializer","handler"})
public class Tag {
    @Id
    @GeneratedValue( strategy = GenerationType.IDENTITY)
    private int id;
    @Lob
    private String content;
    @ManyToMany(mappedBy = "tags",fetch = FetchType.LAZY)
    private List<Book> books;
}
