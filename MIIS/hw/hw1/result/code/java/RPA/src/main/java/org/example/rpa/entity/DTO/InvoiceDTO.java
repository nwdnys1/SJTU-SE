package org.example.rpa.entity.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDate;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class InvoiceDTO {
    private String number;
    private LocalDate date;
    private String customer;
    private String supplier;
    private int amount;
    private String status;
    private String remark;
    private String imageUri;
    private String fileName;
}
