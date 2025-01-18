package org.example.backend.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.example.backend.entity.Book;

import java.util.List;
@AllArgsConstructor
@NoArgsConstructor
@Data
public class PageResponse<T> {
    private long total;
    private int pages;
    private List<T> content;
}
