package org.example.rpa.entity;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDate;

@Entity
@Data
@AllArgsConstructor
@NoArgsConstructor
@Table(name = "invoice")
@JsonIgnoreProperties({"hibernateLazyInitializer", "handler", "customer", "supplier"})
public class Invoice {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer id;
    private String fileName;
    private String imageUri;
    private Integer amount;
    private LocalDate date;
    private Integer status;//1 通过 2 未通过 3 人工审批
    private String remark;//备注 未通过或人工审批的原因
    private String number;//发票号
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "cid")
    private Customer customer;
    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "sid")
    private Supplier supplier;
}
