package org.example.rpa.entity.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class InvoiceSummary {
    private int TotalInvoices;
    private int ApprovedInvoices;
    private int RejectedInvoices;
    private int InvoicesSentforManualReview;
    private ApprovalStatusRatio ApprovalStatusRatio;
    private int MaximumInvoiceAmount;
    private int MinimumInvoiceAmount;
    private int AverageInvoiceAmount;
    private String MostCommonReasonforManualReview;
    private int DuplicateInvoiceCount;
    @Data
    public static class ApprovalStatusRatio {
        private String Approved;
        private String Rejected;
        private String ManualReview;
    }
}
