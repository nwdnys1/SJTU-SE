package org.example.rpa.entity.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class TransactionSummary {
    public static class relation{
        public String client;
        public String supplier;
    }
    private String[] major_clients;
    private String[] clients;
    private String[] regular_clients;
    private String[] important_suppliers;
    private String[] suppliers;
    private String[] regular_suppliers;
    private String[] top_buyers_by_purchase_volume;
    private String[] top_sellers_by_sales_volume;
    private relation most_frequent_transaction_relationship;
}
