package org.example.backend.service.Impl;

import org.example.backend.DAO.BookDAO;
import org.example.backend.DAO.OrderDAO;
import org.example.backend.DTO.PageResponse;
import org.example.backend.DTO.SalesInfo;
import org.example.backend.entity.*;
import org.example.backend.repository.MySQLRepository.UploadRepository;
import org.example.backend.service.BookService;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class BookServiceImpl implements BookService {
    private final BookDAO repository;
    private final UploadRepository uploadRepository;
    private final OrderDAO orderDAO;
    public BookServiceImpl(BookDAO repository, UploadRepository uploadRepository, OrderDAO orderDAO) {
        this.repository = repository;
        this.uploadRepository = uploadRepository;
        this.orderDAO = orderDAO;
    }
    public Result<List<Book>> getBooks(){
        return Result.success(repository.findAll());
    }
    public Result<Book> getBookById(int id){
        if(repository.existsById(id)){
            return Result.success(repository.getBookById(id));
        }else{
            return Result.error(404, "书籍不存在！");
        }
    }
    public Result<List<Book>> getRecommendations(int nums){
        return Result.success(repository.getBooksByRatingGreaterThanOrderByRatingDesc(BigDecimal.valueOf(0)).subList(0, nums));
    }
    public Result<Book> addBook(Book book){
        book.setRating(BigDecimal.valueOf(0));
        book.setSales(0);
        return Result.success(repository.save(book));
    }
    public Result<Book> updateBook(int id, Book book){
        Book oldBook = repository.findById(id);
        if(oldBook == null){
            return Result.error(404, "书籍不存在！");
        }
        oldBook.setTitle(book.getTitle());
        oldBook.setAuthor(book.getAuthor());
        //oldBook.setDescription(book.getDescription());
        //oldBook.setPrice(book.getPrice());
        //oldBook.setRating(book.getRating());
        oldBook.setStock(book.getStock());
        oldBook.setISBN(book.getISBN());
        return Result.success(repository.save(oldBook));
    }
    public Result<Book> deleteBook(int id){
        if(repository.existsById(id)){
            if(!repository.getBookById(id).getCartItems().isEmpty()){
                return Result.error(400, "书籍已被用户加入购物车，无法删除！");
            }
            if(!repository.getBookById(id).getOrderItems().isEmpty()){
                return Result.error(400, "书籍已被用户购买，无法删除！");
            }
            repository.deleteById(id);
            return Result.success(null);
        }else{
            return Result.error(404, "书籍不存在！");
        }
    }
    public Result<PageResponse<Book>> searchBooks(String keyword, int page, int pageSize){
        Pageable pageable = PageRequest.of(page - 1, pageSize);
        Page<Book> books = repository.getBooksByTitleLikeOrAuthorLike("%" + keyword + "%", "%" + keyword + "%", pageable);
        PageResponse<Book> response = new PageResponse<Book>(
                books.getTotalElements(),
                books.getTotalPages(),
                books.getContent()
        );
        return Result.success(response);
    }
    public Result<PageResponse<Book>> categorySearch(int tid, int page, int pageSize){
        Pageable pageable = PageRequest.of(page - 1, pageSize);
        Tag tag = new Tag();
        tag.setId(tid);
        Page<Book> books = repository.findBooksByTagsContains(tag, pageable);
        PageResponse<Book> response = new PageResponse<Book>(
                books.getTotalElements(),
                books.getTotalPages(),
                books.getContent()
        );
        return Result.success(response);
    }
    public Result<String> updateCover(int id, MultipartFile file) {
        Book book = repository.findById(id);
        if (book == null) {
            return Result.error(404, "书籍不存在！");
        }
        try {
            String url = uploadRepository.uploadFile(file, "image");
            book.setCover(url);
            repository.save(book);
            return Result.success(url);
        }
        catch (IOException e) {
            return Result.error(500, e.getMessage());
        }
    }
    public Result<List<SalesInfo>> rank(LocalDateTime start, LocalDateTime end,int nums){
        LinkedHashMap<Integer,SalesInfo> saleInfos = new LinkedHashMap<>();
        List<Order> orders= orderDAO.getOrdersByCreateTimeAfterAndCreateTimeBefore(start, end);//找到所有时间段内的订单
        for(Order order: orders){
            List<OrderItem> orderItems = order.getOrderItems();
            for(OrderItem orderItem: orderItems){
               Integer bid = orderItem.getBook().getId();
                if(saleInfos.containsKey(bid)){
                   saleInfos.get(bid).setSales(saleInfos.get(bid).getSales() + orderItem.getQuantity());
                }else{
                    saleInfos.put(bid, new SalesInfo(bid, orderItem.getBook().getTitle(), orderItem.getQuantity()));
                }
            }
        }
        List<SalesInfo> list = new ArrayList<>(saleInfos.values());
        list.sort((a, b) -> b.getSales() - a.getSales());//按销量降序排列
        return Result.success(list.subList(0, Math.min(nums, list.size())));
    }
}
