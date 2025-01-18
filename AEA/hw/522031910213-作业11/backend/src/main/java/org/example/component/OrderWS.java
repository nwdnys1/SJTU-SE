package org.example.component;

import jakarta.websocket.*;
import jakarta.websocket.server.ServerEndpoint;
import lombok.extern.slf4j.Slf4j;
import org.example.DAO.UserDAO;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.io.IOException;

import java.util.concurrent.ConcurrentHashMap;

@ServerEndpoint("/ws/order")
@Component
@Slf4j
public class OrderWS {
    private static UserDAO userDAO;
    @Autowired
    public void setUserDAO(UserDAO userDAO) {
        OrderWS.userDAO = userDAO;
    }
    private static final ConcurrentHashMap<Integer, Session> sessionPool = new ConcurrentHashMap<>();//用户id和session的映射
    private int getUid(Session session) {//根据session获取用户id
        return userDAO.getUserByUsername(session.getUserPrincipal().getName()).getId();
    }
    public void SendToUser (int uid, String message){
        Session session = sessionPool.get(uid);
        if (session == null) {
            log.info("USER {} 不在线", uid);
            return;
        }
        try {
            session.getBasicRemote().sendText(message);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /**
     * 链接成功调用的方法
     */
    @OnOpen
    public void onOpen(Session session) throws IOException {
        int uid = getUid(session);
        sessionPool.put(uid, session);//将用户id和session绑定
        log.info("与USER {} 建立连接", uid);
    }
    /**
     * 链接关闭调用的方法
     */
    @OnClose
    public void onClose(Session session) {
        int uid = getUid(session);
        sessionPool.remove(uid);//移除用户id和session的绑定
        log.info("关闭与USER {} 的连接", uid);
    }
    /**
     * 收到客户端消息后调用的方法
     */
    @OnMessage
    public void onMessage(String message,Session session) throws IOException {
        log.info("来自USER {} 的消息:{}", getUid(session), message);
    }

    /**
     * 发送错误时的处理
     */
    @OnError
    public void onError(Session session, Throwable error) {
        log.error("发生错误");
        log.info( error.getMessage());
    }
}
