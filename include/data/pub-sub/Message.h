/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/6 上午 08:10
 */

// Message.h
#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType {
    NETWORK_STATUS_CHANGE = 0,
    // 可以在这里添加更多的事件类型
};

class Message {
public:
    virtual ~Message() {

    }
    // 可以添加一些通用的虚函数，供派生类实现
};

#endif
