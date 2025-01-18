package org.example.backend.config;


import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.ResourceHandlerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;

@Configuration
public class UploadConfig implements WebMvcConfigurer {
    @Override
    public void addResourceHandlers(ResourceHandlerRegistry registry) {
        //前面的upload意思是，访问的时候路径上要加上upload，不然也访问不到。
        //后面的upload意思是，你图片上传的路径，我的图片上传就在upload文件中(新创建的的文件夹)。
        registry.addResourceHandler("/image/**") // 设置映射路径
                .addResourceLocations("file:src/main/resources/static/image/"); // 设置资源路径


        registry.addResourceHandler("/document/**") // 设置映射路径
                .addResourceLocations("file:src/main/resources/static/document/"); // 设置资源路径

    }
}



