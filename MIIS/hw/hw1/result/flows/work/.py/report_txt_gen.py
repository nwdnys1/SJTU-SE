import json

def generate_invoice_report(json_invoice_file, json_transaction_file, output_txt_file):
    # 从文件中读取发票 JSON 数据
    with open(json_invoice_file, 'r', encoding='utf-8') as f:
        invoice_data = json.load(f)

    # 从文件中读取交易 JSON 数据
    with open(json_transaction_file, 'r', encoding='utf-8') as f:
        transaction_data = json.load(f)

    # 提取发票信息
    total_invoices = invoice_data["InvoiceApprovalSummary"]["totalInvoices"]
    approved_invoices = invoice_data["InvoiceApprovalSummary"]["approvedInvoices"]
    rejected_invoices = invoice_data["InvoiceApprovalSummary"]["rejectedInvoices"]
    manual_review_count = invoice_data["InvoiceApprovalSummary"]["invoicesSentforManualReview"]

    # 提取交易信息
    top_buyers = transaction_data["transaction_summary"]["top_buyers_by_purchase_volume"]
    top_sellers = transaction_data["transaction_summary"]["top_sellers_by_sales_volume"]

    # 创建报告文本
    report_message = (
        "尊敬的主管部门：\n\n"
        "您好！\n\n"
        "\t本批发票已通过自动化完成处理，部分关键数据如下：\n\n"
        f"\t总发票数量：{total_invoices}\n"
        f"\t通过发票数量：{approved_invoices}\n"
        f"\t不通过发票数量：{rejected_invoices}\n"
        f"\t转人工审批发票数量：{manual_review_count}\n"
        "\t当前购买量最大的3个买方：{}\n".format("\t".join(top_buyers)) +
        "\t当前卖出量最大的3个卖方：{}\n".format("\t".join(top_sellers)) +
        "\n\t附件中的报表统计信息包括了详细的交易关系概况，审批状态概况，"
        "\t发票数据详情，买方数据详情和卖方数据详情。\n"
        "\t同时也附上了买方开销分布，卖方收入分布与发票金额分布的数据分析图，请查阅。\n\n"
        "如有任何问题，请随时与我们邮件联系。\n\n"
        "祝好！"
    )

    # 将报告写入 TXT 文件
    with open(output_txt_file, 'w', encoding='utf-8') as txt_file:
        txt_file.write(report_message)

# 调用函数并传入 JSON 文件路径和输出 TXT 文件路径
generate_invoice_report('./.json/invoice_approval_summary.json', './.json/transaction_summary.json', './.txt/report_body.txt')
