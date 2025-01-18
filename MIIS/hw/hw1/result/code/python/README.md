## 运行环境

python3.8

## 项目目录介绍

### requirements.txt

项目依赖包管理文件<br>
首次下载此项目后执行`pip3 install -r requirements.txt`即下载安装依赖包

### data.py

根据接口文档自动生成的相关配置（请根据接口文档配置所需的请求参数）

- 请求协议
- 请求地址
- 响应数据段，便于程序处理接口响应
- APPId、APIKey、APISecret 信息

### main.py

程序的执行入口

### resource 目录

此目录提供平台内置的请求文件以及后续保存程序执行响应文件

### sample 目录

程序核心执行逻辑步骤

#### exception.py

自定义异常

#### ne_utils.py

工具文件

- 读取问题获取文件内容
- 清空目录
- 生成鉴权的 url
- 解析 url 获取对应的 host、path、schema

#### aipaas_client.py

核心程序执行

- 数据准备 prepare_req_data
- 执行 execute
- 处理响应数据 deal_response

[{"orientation": 0, "flag": 0, "code": 0, "confidence": {"titleConfidence": 0.915, "purchaserNameConfidence": 0.996, "purchaserTaxNoConfidence": 1.0, "purchaserAddressPhoneConfidence": 0.993, "purchaserBankConfidence": 0.985, "salesNameConfidence": 0.999, "salesTaxNoConfidence": 0.976, "salesAddressPhoneConfidence": 1.0, "salesBankAndNoConfidence": 0.985, "amountTaxCNConfidence": 0.959, "amountTaxConfidence": 1.0, "taxConfidence": 1.0, "receiverNameConfidence": 1.0, "recheckNameConfidence": 1.0, "drawerNameConfidence": 1.0, "priInvoiceCodeConfidence": 0.993, "priInvoiceNumberConfidence": 1.0, "aftInvoiceCodeConfidence": 1.0, "aftInvoiceNumberConfidence": 1.0, "invoiceFormConfidence": 1.0, "redSealConfidence": 1.0, "sealMarkConfidence": 0.937}, "dataCode": -10001, "regionFourPoint": [3, 118, 1441, 118, 1441, 1003, 3, 1003], "dataMsg": "未调用发票核验", "coord": {"titleCoord": [568, 166, 937, 163, 937, 207, 568, 210], "invoiceCodeCoord": [265, 194, 490, 194, 490, 234, 265, 234], "invoiceNumberCoord": [1010, 183, 1271, 183, 1271, 232, 1010, 232], "billingDateCoord": [1193, 264, 1360, 264, 1360, 295, 1193, 295], "purchaserNameCoord": [350, 327, 604, 327, 604, 349, 350, 349], "purchaserTaxNoCoord": [371, 360, 642, 360, 642, 383, 371, 383], "purchaserAddressPhoneCoord": [347, 394, 818, 394, 818, 415, 347, 415], "purchaserBankCoord": [350, 426, 842, 427, 842, 450, 350, 448], "salesNameCoord": [350, 794, 564, 790, 564, 814, 350, 818], "salesTaxNoCoord": [378, 824, 708, 821, 708, 844, 378, 848], "salesAddressPhoneCoord": [351, 856, 802, 854, 802, 873, 351, 875], "salesBankAndNoCoord": [351, 886, 728, 883, 728, 905, 351, 910], "amountTaxCNCoord": [490, 741, 735, 741, 735, 769, 490, 769], "amountTaxCoord": [1145, 741, 1285, 738, 1286, 767, 1145, 770], "receiverNameCoord": [245, 913, 299, 913, 299, 946, 245, 946], "recheckNameCoord": [580, 910, 631, 910, 631, 940, 580, 940], "drawerNameCoord": [881, 909, 957, 909, 957, 940, 881, 940], "passwordFieldCoord": [901, 329, 1332, 327, 1336, 431, 903, 431], "remarksCoord": [903, 791, 1233, 791, 1233, 814, 903, 814], "invoiceListsCoord": [[148, 489, 441, 489, 441, 519, 148, 519], [467, 493, 523, 493, 523, 517, 467, 517], [649, 493, 671, 493, 671, 515, 649, 515], [802, 493, 820, 493, 820, 514, 802, 514], [840, 493, 950, 493, 950, 514, 840, 514], [1066, 491, 1139, 491, 1139, 511, 1066, 511], [1167, 491, 1199, 491, 1199, 510, 1167, 510], [1323, 493, 1383, 493, 1383, 508, 1323, 508]], "priInvoiceCodeCoord": [265, 194, 490, 194, 490, 234, 265, 234], "priInvoiceNumberCoord": [1010, 183, 1271, 183, 1271, 232, 1010, 232], "aftInvoiceCodeCoord": [1278, 203, 1382, 206, 1381, 230, 1278, 228], "aftInvoiceNumberCoord": [1262, 230, 1379, 230, 1379, 260, 1262, 260], "amountTaxOCRCoord": [1145, 741, 1285, 738, 1286, 767, 1145, 770], "invoiceFormCoord": [-1, -1, -1, -1, -1, -1, -1, -1], "redSealCoord": [1071, 762, 1342, 762, 1342, 992, 1071, 992], "oilMarkCoord": [-1, -1, -1, -1, -1, -1, -1, -1], "invTaxSignCoord": [-1, -1, -1, -1, -1, -1, -1, -1], "tollSignCoord": [-1, -1, -1, -1, -1, -1, -1, -1], "sealMarkCoord": [1071, 910, 1234, 909, 1234, 943, 1071, 943], "invoiceFormNumCoord": [-1, -1, -1, -1, -1, -1, -1, -1], "qrListsCoord": [261, 195, 263, 309, 149, 310, 149, 197], "totalAmountCoord": [1013, 698, 1145, 698, 1145, 726, 1013, 726], "totalTaxCoord": [1276, 697, 1390, 695, 1390, 724, 1277, 726], "totalAmountOCRCoord": [1013, 698, 1145, 698, 1145, 726, 1013, 726]}, "imgOrgsize": [1070, 1443], "regionFourPointOri": [3, 118, 1441, 118, 1441, 1003, 3, 1003], "invoice": {"administrativeDivisionName": "广东省", "title": "广东增值税专用发票", "invoiceCode": "4400154130", "invoiceNumber": "12271524", "billingDate": "2016-06-12", "billingDateocr": "2016-06-12", "totalAmount": "2987.18", "checkCode": "", "amountTax": "3495.00", "amountTaxCN": "叁仟肆佰玖拾伍圆整", "totalAmountocr": "2987.18", "amountTaxocr": "3495.00", "priInvoiceCode": "4400154130", "priInvoiceNumber": "12271524", "aftInvoiceCode": "4400154130", "aftInvoiceNumber": "12271524", "purchaserName": "深圳市购机汇网络有限公司", "purchaserTaxNo": "440300083885931", "purchaserAddressPhone": "深圳市龙华新区民治街道民治大道展酒科技大厦 A12070755-23806606", "purchaserBank": "中国工商银行股份有限公司深圳园岭支行 4000024709200172809", "salesName": "广州晶东贸易有限公司", "salesTaxNo": "91440101664041243T", "salesAddressPhone": "广州市黄埔区九龙镇九龙工业园凤凰三横路 99 号 66215500", "salesBankAndNo": "工行北京路支行 3602000919200384952", "passwordField": "<<1<//3*26-++936-9<9*575>39-<5//81>84974<00+7>2*0*53-++125*++9+-///5-7+/-0>8<98155<3/8*+//81/84+>6>4\*36>4538", "totalTax": "507.82", "machineCode": "", "remarks": "dd42982413947(00001,19522879996", "receiverName": "王梅", "recheckName": "张雪", "drawerName": "陈秋燕", "invoiceForm": "抵扣联", "invoiceFormNum": "第二联", "travelTax": "", "redSeal": "1", "oilMark": "0", "invTaxSign": "1", "tollSign": "0", "sealMark": "1", "kind": "其他", "invoiceLists": [{"commodityName": "小米红米 3 全网通版时尚金色移动联通电信 4G 手机双卡双待", "specificationModel": "红米 3", "unit": "个", "quantity": "5", "unitPrice": "597.43589744", "amount": "2987.18", "taxRate": "0.17", "tax": "507.82", "taxPercentage": "17%"}], "invoiceType": 1}, "region": [0, 113, 1443, 1008], "qrLists": {"invoiceCode": "4400154130", "invoiceNumber": "12271524", "billingDate": "2016-06-12", "salesTaxNo": "", "qrInfo": "01,01,4400154130,12271524,2987.18,20160612,,688C,", "totalAmount": "2987.18"}}]
