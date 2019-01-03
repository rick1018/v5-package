﻿// PHZ
// 2018-5-15

// PHZ
// 2018-5-15

#ifndef XOP_TCPSERVER_H
#define XOP_TCPSERVER_H

#include <memory>
#include <string>
#include <unordered_map>
#include "TcpConnection.h"
#include "Acceptor.h"

namespace xop
{

class Acceptor;
class EventLoop;

class TcpServer 
{
public:	
    TcpServer(EventLoop* loop, std::string ip, uint16_t port);
    virtual ~TcpServer();  

    void setMessageCallback(const MessageCallback& cb)
    { _messageCallback = cb; }

protected:
    void newConnection(SOCKET sockfd);
    void removeConnection(TcpConnectionPtr& conn);

    EventLoop* _eventLoop; 
    std::shared_ptr<Acceptor> _acceptor; 
    MessageCallback _messageCallback;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> _connections;
};

}

#endif 
