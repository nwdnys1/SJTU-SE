package org.example.rpa.repo;

import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.model.ObjectMetadata;
import com.amazonaws.services.s3.model.PutObjectRequest;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Repository;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.UUID;

@Repository
public class UploadRepo {

    private static final String UPLOAD_DIR = "/root/ebookstore/static/image/";//如果上传到服务器上 用这个
    @Value("${s3.domain}")
    private String DOMAIN ;//上传到个人图床 用这个
    private final AmazonS3 amazonS3;
    @Value("${s3.bucket_name}")
    private String bucketName;
    public UploadRepo(AmazonS3 amazonS3) {
        this.amazonS3 = amazonS3;
    }
    public String uploadFile(MultipartFile file,String type) throws IOException {
        if (file.isEmpty()) {
            return null;
        }
        // 生成一个唯一的文件名
        String prefix =UUID.randomUUID().toString()+"_";
        String originalFilename = file.getOriginalFilename().replace("%", "");//去掉特殊字符
        if (originalFilename.length() > 100)
            originalFilename = originalFilename.substring(0,100);//限制文件名长度
        String encodedFileName = URLEncoder.encode(prefix + originalFilename, StandardCharsets.UTF_8);
        encodedFileName = encodedFileName.replace("+", "%20");//替换空格
        String fileName = prefix + originalFilename;
        // 设置文件的元信息 否则默认application/octet-stream 会下载
        ObjectMetadata metadata = new ObjectMetadata();
        metadata.setContentType(file.getContentType());
        metadata.setContentLength(file.getSize());
        // 保存文件
        amazonS3.putObject(new PutObjectRequest(bucketName, fileName, file.getInputStream(), metadata));
        // 返回文件的url
        return DOMAIN +'/'+ encodedFileName;
    }
}
