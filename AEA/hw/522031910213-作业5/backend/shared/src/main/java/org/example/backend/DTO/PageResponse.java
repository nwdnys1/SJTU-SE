package org.example.backend.DTO;

import java.util.List;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
@AllArgsConstructor
@NoArgsConstructor
@Data
public class PageResponse<T> {
    private long total;
    private int pages;
    private List<T> content;
}
