package org.example.rpa;


import com.alibaba.fastjson2.JSON;
import com.alibaba.fastjson2.JSONObject;
import io.github.cdimascio.dotenv.Dotenv;
import org.example.rpa.entity.*;
import org.example.rpa.entity.DTO.InvoiceDTO;
import org.example.rpa.entity.DTO.InvoiceSummary;
import org.example.rpa.entity.DTO.TransactionSummary;
import org.example.rpa.repo.*;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.mock.web.MockMultipartFile;
import org.springframework.web.multipart.MultipartFile;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.time.LocalDate;
import java.util.*;


@SpringBootApplication
public class RpaApplication implements CommandLineRunner {
    private final List<JSONObject> data = new ArrayList<>();
    private final List<Customer> customers = new ArrayList<>();
    private final List<Supplier> suppliers = new ArrayList<>();
    private final List<Invoice> invoices = new ArrayList<>();
    private final List<NonInvoice> nonInvoices = new ArrayList<>();
    private final int[] invoiceTypes = {1, 4, 14, 11, 24, 25, 28, 8, 3, 36, 10, 15, 23, 21, 9};
    private final UploadRepo uploadRepo;
    private final CustomerRepo customerRepo;
    private final SupplierRepo supplierRepo;
    private final InvoiceRepo invoiceRepo;
    private final NonInvoiceRepo nonInvoiceRepo;
    private final FrequencyRepo frequencyRepo;
    @Value("${file.path}")
    private String filePath = "data.json";
    private int duplicateNum = 0;

    public RpaApplication(UploadRepo uploadRepo, CustomerRepo customerRepo, SupplierRepo supplierRepo, InvoiceRepo invoiceRepo, NonInvoiceRepo nonInvoiceRepo, FrequencyRepo frequencyRepo) {
        this.uploadRepo = uploadRepo;
        this.customerRepo = customerRepo;
        this.supplierRepo = supplierRepo;
        this.invoiceRepo = invoiceRepo;
        this.nonInvoiceRepo = nonInvoiceRepo;
        this.frequencyRepo = frequencyRepo;
    }

    public static void main(String[] args) {
        Dotenv dotenv = Dotenv.load(); // 加载 .env 文件
        // 将环境变量设置为系统属性
        System.setProperty("S3_BUCKET_NAME", dotenv.get("S3_BUCKET_NAME"));
        System.setProperty("S3_ACCESS_KEY", dotenv.get("S3_ACCESS_KEY"));
        System.setProperty("S3_SECRET_KEY", dotenv.get("S3_SECRET_KEY"));
        System.setProperty("S3_ENDPOINT", dotenv.get("S3_ENDPOINT"));
        System.setProperty("S3_REGION", dotenv.get("S3_REGION"));
        System.setProperty("S3_DOMAIN", dotenv.get("S3_DOMAIN"));
        System.setProperty("DB_URL", dotenv.get("DB_URL"));
        System.setProperty("DB_USERNAME", dotenv.get("DB_USERNAME"));
        System.setProperty("DB_PASSWORD", dotenv.get("DB_PASSWORD"));

        SpringApplication.run(RpaApplication.class, args);
    }

    private static void calculateCustomerScores(List<Customer> sortedCustomers, double[] scores, String by) {
        int size = sortedCustomers.size();
        for (int i = 0; i < size; i++) {
            if (i > 0 && by == "amount" && sortedCustomers.get(i).getAmount() == sortedCustomers.get(i - 1).getAmount() ||
                    i > 0 && by == "count" && sortedCustomers.get(i).getCount() == sortedCustomers.get(i - 1).getCount()) {
                // 如果相等，分数与前一个相同
                scores[i] = scores[i - 1];
            } else {
                // 否则，根据当前排名计算分数
                scores[i] = (double) (size - i - 1) / (size - 1) * 100; // 0到100评分
                System.out.println(sortedCustomers.get(i).getName() + " Score: " + scores[i]);
            }
        }
    }

    private static void calculateSupplierScores(List<Supplier> sortedSuppliers, double[] scores, String by) {
        int size = sortedSuppliers.size();
        for (int i = 0; i < size; i++) {
            if (i > 0 && by == "amount" && sortedSuppliers.get(i).getAmount() == sortedSuppliers.get(i - 1).getAmount() ||
                    i > 0 && by == "count" && sortedSuppliers.get(i).getCount() == sortedSuppliers.get(i - 1).getCount()) {
                // 如果相等，分数与前一个相同
                scores[i] = scores[i - 1];
            } else {
                // 否则，根据当前排名计算分数
                scores[i] = (double) (size - i - 1) / (size - 1) * 100; // 0到100评分
            }
            System.out.println(sortedSuppliers.get(i).getName() + " Score: " + scores[i]);
        }
    }

    @Override
    public void run(String... args) throws Exception {
        //删除待人工审批发票文件夹下的所有文件
        Path path = Paths.get("../.jpg/待人工审批发票/");
        if (Files.exists(path) && Files.isDirectory(path)) {
            // 遍历目录，删除所有文件
            try (DirectoryStream<Path> stream = Files.newDirectoryStream(path)) {
                for (Path entry : stream) {
                    Files.delete(entry); // 删除文件
                }
            }
        } else {
            System.out.println("目录不存在或不是有效的目录");
        }
        data.addAll(readData());
        parseData();
        saveData();
        System.exit(0);
    }

    private List<JSONObject> readData() {
        System.out.println("Reading data from file: " + filePath);
        try (InputStream inputStream = new FileInputStream(filePath)) {
            String json = new String(inputStream.readAllBytes(), StandardCharsets.UTF_8);
            return JSON.parseArray(json, JSONObject.class);
        } catch (Exception e) {
            System.out.println("Error reading file: " + e.getMessage());
        }
        return null;
    }

    // 将file转换为multipartFile并上传 返回url
    private String uploadFile(String filePath, Integer dataSet, int status) throws IOException {
        String fileName = filePath;//文件名
        filePath = "../input/" + dataSet.toString() + "/" + fileName; // 修正路径为../input/dataSet/fileName


        //把人工审批的发票导出到指定文件夹
        if (status == 3) {
            Files.copy(Paths.get(filePath), Paths.get("../.jpg/待人工审批发票/" + fileName), StandardCopyOption.REPLACE_EXISTING);
        }


        File file = new File(filePath);
        if (!file.exists()) {
            throw new IOException("File not found: " + filePath);
        }
        FileInputStream fileInputStream = new FileInputStream(file);
        MultipartFile multipartFile = new MockMultipartFile(file.getName(), file.getName(), "image/jpeg", fileInputStream);
        return uploadRepo.uploadFile(multipartFile, "image");
    }

    private void parseData() throws IOException {
        for (JSONObject json : data) {
            int code = json.getJSONObject("header").getIntValue("code"); // 错误码
            int dataSet = json.getIntValue("data_set"); // 数据集类型
            String fileName = json.getString("file_name");
            if (code != 0) { // 有错误码
                Invoice invoice = new Invoice();
                invoice.setStatus(3); // 人工审批
                invoice.setRemark("无法识别发票 错误码：{" + code + "} 人工审批");
                invoice.setImageUri(uploadFile(fileName, dataSet, 3));
                invoice.setFileName(fileName);
                invoiceRepo.save(invoice);
                continue;
            }
            // 获取base64编码的结果
            String base64 = json.getJSONObject("payload").getJSONObject("output_text_result").getString("text");
            JSONObject result = JSON.parseArray(new String(java.util.Base64.getDecoder().decode(base64))).getJSONObject(0);
            if (result.containsKey("invoice")) {
                result = result.getJSONObject("invoice"); // 有invoice字段
            }
            System.out.println(result.toJSONString());
            int type = result.getIntValue("invoiceType");
            if (Arrays.stream(invoiceTypes).noneMatch(i -> i == type)) { // 非发票
                NonInvoice nonInvoice = new NonInvoice();
                nonInvoice.setImageUri(uploadFile(fileName, dataSet, 0));
                nonInvoice.setFileName(fileName);
                nonInvoiceRepo.save(nonInvoice);
            } else { // 发票
                Invoice invoice = new Invoice();
                String number = result.getString("invoiceNumber");//发票号码
                if (number != null && invoiceRepo.getInvoiceByNumber(number) != null) { // 重复发票 不再处理
                    duplicateNum++;
                    continue;
                }
                String dateStr = result.getString("billingDate");
                String supplierName = result.getString("salesName");//供应商名称
                String customerName = result.getString("purchaserName");//客户名称
                Integer amount = result.getInteger("amountTax");//金额
                filter(amount, supplierName, customerName, dateStr, invoice);
                if (invoice.getStatus() != null && invoice.getStatus() == 3) { // 人工审批
                    invoice.setImageUri(uploadFile(fileName, dataSet, 3));
                    invoice.setFileName(fileName);
                    invoiceRepo.save(invoice);
                    continue;
                }
                invoice.setNumber(number);
                invoice.setAmount(amount);

                Supplier supplier = supplierRepo.getSupplierByName(supplierName);
                if (supplier == null) {
                    supplier = new Supplier();
                    supplier.setName(supplierName);
                    supplier.setAmount(0);
                    supplier.setCount(0);
                }
                supplier.setAmount(supplier.getAmount() + amount);
                supplier.setCount(supplier.getCount() + 1);
                supplierRepo.save(supplier);
                invoice.setSupplier(supplier);

                Customer customer = customerRepo.getCustomerByName(customerName);
                if (customer == null) {
                    customer = new Customer();
                    customer.setName(customerName);
                    customer.setAmount(0);
                    customer.setCount(0);
                }

                customer.setAmount(customer.getAmount() + amount);
                customer.setCount(customer.getCount() + 1);
                customerRepo.save(customer);
                invoice.setCustomer(customer);

                //
                Frequency frequency = frequencyRepo.getFrequencyByCustomerIdAndSupplierId(customer.getId(), supplier.getId());
                if (frequency == null) {
                    frequency = new Frequency();
                    frequency.setCustomer(customer);
                    frequency.setSupplier(supplier);
                    frequency.setFreq(0);
                }
                frequency.setFreq(frequency.getFreq() + 1);
                frequencyRepo.save(frequency);

                //进行审批
                approve(dataSet, invoice);
                invoice.setImageUri(uploadFile(fileName, dataSet, invoice.getStatus()));
                invoice.setFileName(fileName);
                invoiceRepo.save(invoice);
            }
        }
    }
    // 导出数据

    //筛选出无法识别字段的发票 转为人工审批
    private void filter(Integer amount, String supplierName, String customerName, String dateStr, Invoice invoice) {
        if (amount == null || amount <= 0) {
            invoice.setStatus(3);
            invoice.setRemark("无法识别金额 人工审批");
            return;
        }
        if (supplierName == null || supplierName.isEmpty()) {
            invoice.setStatus(3);
            invoice.setRemark("无法识别供应商 人工审批");
            return;
        }
        if (customerName == null || customerName.isEmpty()) {
            invoice.setStatus(3);
            invoice.setRemark("无法识别客户 人工审批");
            return;
        }
        if (dateStr == null || dateStr.isEmpty()) {
            invoice.setStatus(3);
            invoice.setRemark("无法识别开票日期 人工审批");
            return;
        }
        LocalDate date = null;
        try {
            date = LocalDate.parse(dateStr);
        } catch (Exception e) {
            invoice.setStatus(3);
            invoice.setRemark("无法识别开票日期 人工审批");
        }
        invoice.setDate(date);
    }

    // 审批通过识别的发票 dataSet是数据集类型
    private void approve(int dataSet, Invoice invoice) {
        if (dataSet == 1) {
            Customer customer = invoice.getCustomer();
            String customerName = customer.getName();
            if (!Objects.equals(customerName, "浙江大学")) {
                int same = customerName.contains("浙") ? 1 : 0;
                same += customerName.contains("江") ? 1 : 0;
                same += customerName.contains("大") ? 1 : 0;
                same += customerName.contains("学") ? 1 : 0;
                if (same < 3) { // 名字至少包含3个字
                    invoice.setStatus(2);
                    invoice.setRemark("付款方不是浙江大学");
                } else {
                    invoice.setStatus(3);
                    invoice.setRemark("付款方疑似识别不清 人工审批");
                }
                return;
            }
            LocalDate date = invoice.getDate();
            if (date.getYear() != 2015) {
                invoice.setStatus(2);
                invoice.setRemark("开票日期不是2015年");
                return;
            }
            if (invoice.getAmount() > 1600) {
                invoice.setStatus(2);
                invoice.setRemark("金额超过1600元");
                return;
            }
            invoice.setStatus(1);
        }
        if (dataSet == 2) {
            Customer customer = invoice.getCustomer();
            String customerName = customer.getName();
            if (!Objects.equals(customerName, "深圳市购机汇网络有限公司")) {
                invoice.setStatus(2);
                invoice.setRemark("付款方不是深圳市购机汇网络有限公司");
                return;
            }
            LocalDate date = invoice.getDate();
            if (!Objects.equals(date, LocalDate.of(2016, 6, 12))) {
                invoice.setStatus(2);
                invoice.setRemark("开票日期不是2016年6月12日");
                return;
            }
            if (invoice.getAmount() > 2700) {
                invoice.setStatus(2);
                invoice.setRemark("金额超过2700元");
                return;
            }
            invoice.setStatus(1);
        }
    }

    protected void saveData() throws IOException {
        FileWriter fileWriter = new FileWriter("../.json/buyer.json");
        // customers
        customers.addAll(customerRepo.findAll());
        classifyCustomer(customers);
        fileWriter.write(JSON.toJSONString(customers));
        fileWriter.close();
        // suppliers
        fileWriter = new FileWriter("../.json/seller.json");
        suppliers.addAll(supplierRepo.findAll());
        classifySupplier(suppliers);
        fileWriter.write(JSON.toJSONString(suppliers));
        fileWriter.close();
        // invoices
        fileWriter = new FileWriter("../.json/invoice.json");
        invoices.addAll(invoiceRepo.findAll());
        List<InvoiceDTO> invoiceDTOS = new ArrayList<>();
        for (Invoice invoice : invoices) {
            InvoiceDTO invoiceDTO = new InvoiceDTO();
            if (invoice.getAmount() != null)
                invoiceDTO.setAmount(invoice.getAmount());
            if (invoice.getCustomer() != null)
                invoiceDTO.setCustomer(invoice.getCustomer().getName());
            if (invoice.getSupplier() != null)
                invoiceDTO.setSupplier(invoice.getSupplier().getName());
            if (invoice.getDate() != null)
                invoiceDTO.setDate(invoice.getDate());
            if (invoice.getStatus() != null)
                invoiceDTO.setStatus(invoice.getStatus() == 1 ? "通过" : invoice.getStatus() == 2 ? "未通过" : "人工审批");//1 通过 2 未通过 3 人工审批
            if (invoice.getRemark() != null)
                invoiceDTO.setRemark(invoice.getRemark());
            if (invoice.getNumber() != null)
                invoiceDTO.setNumber(invoice.getNumber());
            if (invoice.getImageUri() != null)
                invoiceDTO.setImageUri(invoice.getImageUri());
            invoiceDTOS.add(invoiceDTO);
        }
        fileWriter.write(JSON.toJSONString(invoiceDTOS));
        fileWriter.close();
        // transaction summary
        fileWriter = new FileWriter("../.json/transaction_summary.json");
        TransactionSummary transactionSummary = new TransactionSummary();
        transactionSummary.setMajor_clients(customers.stream().filter(c -> c.getType().equals("大客户")).map(Customer::getName).toArray(String[]::new));
        transactionSummary.setClients(customers.stream().filter(c -> c.getType().equals("客户")).map(Customer::getName).toArray(String[]::new));
        transactionSummary.setRegular_clients(customers.stream().filter(c -> c.getType().equals("一般客户")).map(Customer::getName).toArray(String[]::new));
        transactionSummary.setImportant_suppliers(suppliers.stream().filter(s -> s.getType().equals("重要供应商")).map(Supplier::getName).toArray(String[]::new));
        transactionSummary.setSuppliers(suppliers.stream().filter(s -> s.getType().equals("供应商")).map(Supplier::getName).toArray(String[]::new));
        transactionSummary.setRegular_suppliers(suppliers.stream().filter(s -> s.getType().equals("一般供应商")).map(Supplier::getName).toArray(String[]::new));
        transactionSummary.setTop_buyers_by_purchase_volume(customers.stream().sorted(Comparator.comparingInt(Customer::getAmount).reversed()).limit(3).map(Customer::getName).toArray(String[]::new));
        transactionSummary.setTop_sellers_by_sales_volume(suppliers.stream().sorted(Comparator.comparingInt(Supplier::getAmount).reversed()).limit(3).map(Supplier::getName).toArray(String[]::new));
        Frequency mostFrequent = frequencyRepo.findAll().stream().max(Comparator.comparingInt(Frequency::getFreq)).orElse(null);
        if (mostFrequent != null) {
            TransactionSummary.relation relation = new TransactionSummary.relation();
            relation.client = mostFrequent.getCustomer().getName();
            relation.supplier = mostFrequent.getSupplier().getName();
            transactionSummary.setMost_frequent_transaction_relationship(relation);
        }
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("transaction_summary", transactionSummary);
        fileWriter.write(JSON.toJSONString(jsonObject));
        fileWriter.close();
        // invoice summary
        fileWriter = new FileWriter("../.json/invoice_approval_summary.json");
        InvoiceSummary invoiceSummary = new InvoiceSummary();
        invoiceSummary.setTotalInvoices(invoices.size());
        invoiceSummary.setApprovedInvoices((int) invoices.stream().filter(i -> i.getStatus() == 1).count());
        invoiceSummary.setRejectedInvoices((int) invoices.stream().filter(i -> i.getStatus() == 2).count());
        invoiceSummary.setInvoicesSentforManualReview((int) invoices.stream().filter(i -> i.getStatus() == 3).count());

        invoiceSummary.setMaximumInvoiceAmount(invoices.stream().filter(i -> i.getAmount() != null).mapToInt(Invoice::getAmount).max().orElse(0));
        invoiceSummary.setMinimumInvoiceAmount(invoices.stream().filter(i -> i.getAmount() != null).mapToInt(Invoice::getAmount).min().orElse(0));
        invoiceSummary.setAverageInvoiceAmount((int) invoices.stream().filter(i -> i.getAmount() != null).mapToInt(Invoice::getAmount).average().orElse(0));
        invoiceSummary.setMostCommonReasonforManualReview(invoices.stream().filter(i -> i.getStatus() == 3).map(Invoice::getRemark).max(Comparator.comparingInt(String::length)).orElse(""));
        invoiceSummary.setDuplicateInvoiceCount(duplicateNum);
        InvoiceSummary.ApprovalStatusRatio approvalStatusRatio = new InvoiceSummary.ApprovalStatusRatio();
        approvalStatusRatio.setApproved(String.format("%.2f%%", (double) invoiceSummary.getApprovedInvoices() / invoiceSummary.getTotalInvoices() * 100));//通过率
        approvalStatusRatio.setRejected(String.format("%.2f%%", (double) invoiceSummary.getRejectedInvoices() / invoiceSummary.getTotalInvoices() * 100));//拒绝率
        approvalStatusRatio.setManualReview(String.format("%.2f%%", (double) invoiceSummary.getInvoicesSentforManualReview() / invoiceSummary.getTotalInvoices() * 100));//人工审批率
        invoiceSummary.setApprovalStatusRatio(approvalStatusRatio);
        jsonObject.clear();
        jsonObject.put("InvoiceApprovalSummary", invoiceSummary);
        fileWriter.write(JSON.toJSONString(jsonObject));
        fileWriter.close();

    }

    private void classifyCustomer(List<Customer> customerList) {
        int size = customerList.size();
        if (size == 1) {
            Customer customer = customerList.get(0);
            customer.setType("客户");
            customerRepo.save(customer);
            return;
        }

        // 1. 根据amount排序并计算评分
        List<Customer> sortedByAmount = new ArrayList<>(customerList);
        sortedByAmount.sort(Comparator.comparingInt(Customer::getAmount).reversed());
        double[] tmp = new double[size];
        double[] amountScores = new double[size];
        RpaApplication.calculateCustomerScores(sortedByAmount, tmp, "amount");
        for (int i = 0; i < size; i++) {
            int index = customerList.indexOf(sortedByAmount.get(i));
            amountScores[index] = tmp[i];
        }

        // 2. 根据count排序并计算评分
        List<Customer> sortedByCount = new ArrayList<>(customerList);
        sortedByCount.sort(Comparator.comparingInt(Customer::getCount).reversed());
        double[] countScores = new double[size];
        tmp = new double[size]; // 重置tmp
        RpaApplication.calculateCustomerScores(sortedByCount, tmp, "count");
        for (int i = 0; i < size; i++) {
            int index = customerList.indexOf(sortedByCount.get(i));
            countScores[index] = tmp[i];
        }

        // 3. 计算加权平均得分
        double[] finalScores = new double[size];
        double amountWeight = 0.7;
        double countWeight = 0.3;
        for (int i = 0; i < size; i++) {
            finalScores[i] = amountWeight * amountScores[i] + countWeight * countScores[i];
            System.out.println(sortedByAmount.get(i).getName() + " 最终得分: " + finalScores[i]);
        }

        // 4. 将客户与最终得分关联
        for (int i = 0; i < size; i++) {
            customerList.get(i).setScore(finalScores[i]);
        }

        // 5. 根据最终得分排序
        customerList.sort(Comparator.comparingDouble(Customer::getScore).reversed());

        // 6. 分类
        int threshold1 = size / 4; // 前25%
        int threshold2 = size * 3 / 4; // 中间50%

        for (int i = 0; i < size; i++) {
            String category;
            if (i < threshold1) {
                category = "大客户";
            } else if (i < threshold2) {
                category = "客户";
            } else {
                category = "一般客户";
            }
            customers.get(i).setType(category);
        }
        customerRepo.saveAll(customers);
    }

    private void classifySupplier(List<Supplier> supplierList) {
        int size = supplierList.size();
        if(size == 1){
            Supplier supplier = supplierList.get(0);
            supplier.setType("供应商");
            supplierRepo.save(supplier);
            return;
        }

        // 1. 根据amount排序并计算评分
        List<Supplier> sortedByAmount = new ArrayList<>(supplierList);
        sortedByAmount.sort(Comparator.comparingInt(Supplier::getAmount).reversed());
        double[] tmp = new double[size];
        RpaApplication.calculateSupplierScores(sortedByAmount, tmp, "amount");
        double[] amountScores = new double[size];
        for (int i = 0; i < size; i++) {
            int index = supplierList.indexOf(sortedByAmount.get(i));
            amountScores[index] = tmp[i];
        }

        // 2. 根据count排序并计算评分
        List<Supplier> sortedByCount = new ArrayList<>(supplierList);
        sortedByCount.sort(Comparator.comparingInt(Supplier::getCount).reversed());
        double[] countScores = new double[size];
        tmp = new double[size];// 重置tmp
        RpaApplication.calculateSupplierScores(sortedByCount, tmp, "count");
        for (int i = 0; i < size; i++) {
            int index = supplierList.indexOf(sortedByCount.get(i));
            countScores[index] = tmp[i];
        }

        // 3. 计算加权平均得分
        double[] finalScores = new double[size];
        double amountWeight = 0.7;
        double countWeight = 0.3;
        for (int i = 0; i < size; i++) {
            finalScores[i] = amountWeight * amountScores[i] + countWeight * countScores[i];
            System.out.println(supplierList.get(i).getName() + " 最终得分: " + finalScores[i]);
        }

        // 4. 将供应商与最终得分关联
        for (int i = 0; i < size; i++) {
            supplierList.get(i).setScore(finalScores[i]);
        }

        // 5. 根据最终得分排序
        supplierList.sort(Comparator.comparingDouble(Supplier::getScore).reversed());

        // 6. 分类
        int threshold1 = size / 4; // 前25%
        int threshold2 = size * 3 / 4; // 中间50%

        for (int i = 0; i < size; i++) {
            String category;
            if (i < threshold1) {
                category = "重要供应商";
            } else if (i < threshold2) {
                category = "供应商";
            } else {
                category = "一般供应商";
            }
            suppliers.get(i).setType(category);
        }
        supplierRepo.saveAll(suppliers);
    }
}
