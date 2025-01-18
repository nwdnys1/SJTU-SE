package org.example.backend.controller;

import org.example.backend.DTO.PageResponse;
import org.example.backend.DTO.SalesInfo;
import org.example.backend.entity.Book;
import org.example.backend.entity.Result;
import org.example.backend.service.BookService;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;

import java.time.LocalDateTime;
import java.util.List;

@RestController
@RequestMapping("/api/book")
public class BookController {

    private final BookService service;
    public BookController(BookService service) {
        this.service = service;
    }
    @GetMapping("/list")
    public Result<List<Book>> list() {
       return service.getBooks();
    }
    @GetMapping("/get/{id}")
    public Result<Book> getBookById(@PathVariable int id) {
       return service.getBookById(id);
    }
    @GetMapping("/recommend/{nums}")
    public Result<List<Book>> getRecommendations(@PathVariable int nums) {//依据rating进行推荐 选取rating最高的前6本书
       return service.getRecommendations(nums);
    }
    @PutMapping("/admin/update/{id}")
    public Result<Book> updateBook(@PathVariable int id, @RequestBody Book book) {
        return service.updateBook(id, book);
    }
    @DeleteMapping("/admin/delete/{id}")
    public Result<Book> deleteBook(@PathVariable int id) {
         return service.deleteBook(id);
    }
    @PostMapping("/admin/add")
    public Result<Book> addBook(@RequestBody Book book) {
       return service.addBook(book);
    }
    @GetMapping("/search")
    public Result<PageResponse<Book>> searchBooks(@RequestParam(defaultValue = "") String keyword, @RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10") int pageSize){
        return service.searchBooks(keyword, page, pageSize);
    }
    @GetMapping("/category")
    public Result<PageResponse<Book>> categorySearch(@RequestParam(defaultValue = "") int tag, @RequestParam(defaultValue = "1") int page, @RequestParam(defaultValue = "10") int pageSize){
        return service.categorySearch(tag, page, pageSize);
    }
    @PostMapping("/cover/{id}")
    public Result<String> uploadCover(@PathVariable int id, @RequestParam("file") MultipartFile file) {
        return service.updateCover(id, file);
    }
    @GetMapping("/admin/rank")//按照指定时间段内的销量进行排名
    public Result<List<SalesInfo>> rank(@RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime start,
                                        @RequestParam @DateTimeFormat(pattern = "yyyy-MM-dd HH:mm:ss") LocalDateTime end,
                                        @RequestParam(defaultValue = "10") int nums) {
        return service.rank(start, end, nums);
    }

}
