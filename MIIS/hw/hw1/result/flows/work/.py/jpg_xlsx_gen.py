import json
import pandas as pd

# 读取 JSON 文件
with open('./.json/invoice.json', 'r', encoding='utf-8') as file:
    invoice_data = json.load(file)

# 提取需要人工审批的发票信息
manual_review_data = []

for invoice in invoice_data:
    if invoice['status'] == '人工审批':
        manual_review_data.append({
            'ImageURL': invoice['imageUri'],
            'ManualReviewReason': invoice['remark']
        })

# 把里面的列名改成中文
manual_review_data = [
    {'图片链接': invoice['ImageURL'], '转人工审批原因': invoice['ManualReviewReason']}
    for invoice in manual_review_data
]

# 创建 DataFrame
df = pd.DataFrame(manual_review_data)

# 写入 Excel 文件
output_file = './.xlsx/转人工审批原因.xlsx'
df.to_excel(output_file, index=False)
