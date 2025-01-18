import json

def generate_manual_review_txt(json_file, output_txt_file):
    # 从文件中读取 JSON 数据
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)

    # 获取待人工审批发票数量
    manual_review_count = data["InvoiceApprovalSummary"]["invoicesSentforManualReview"]

    # 创建待人工审批信息文本
    message = (
        "您好！\n\n"
        "\t在此轮的发票数据处理过程中，我们遇到了一些无法通过自动识别的发票。"
        "\t为了确保合规性和准确性，我们已将这些发票打包转发至您进行人工审批。\n\n"
        "\t以下是需要审批的发票信息，发票的转人工审批原因在附件中列出：\n\n"
        f"\t待人工审批发票数量：{manual_review_count}\n"
        "\t附件：待人工审批发票.7z，转人工审批原因.xlsx\n\n"
        "\t请您在收到后尽快进行审核，并反馈审批结果。如需进一步的信息或帮助，请随时通过邮件与我联系。\n\n"
        "感谢您的配合与支持！\n\n"
        "祝好！"
    )

    # 将信息写入 TXT 文件
    with open(output_txt_file, 'w', encoding='utf-8') as txt_file:
        txt_file.write(message)

# 调用函数并传入 JSON 文件路径和输出 TXT 文件路径
generate_manual_review_txt('./.json/invoice_approval_summary.json', './.txt/jpg_body.txt')
