//
//  main.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include <iostream>
#include "webrtc/base/logging.h"
#include "XMsgClient.h"
#include "XMsgCallback.h"

class MsgServerCallback : public XMsgCallback{
public:
    virtual void OnLogin(int code) {
        LOG(INFO) << __FUNCTION__ << " code:" << code;
    }
    virtual void OnSndMsg(const std::string msg) {
        LOG(INFO) << __FUNCTION__ << " msg:" << msg;
    }
    virtual void OnGetMsg(const std::string from, const std::string msg) {
        LOG(INFO) << __FUNCTION__ << "from:" << from << ", msg:" << msg;
    }
    virtual void OnLogout(int code) {
        LOG(INFO) << __FUNCTION__ << " code:" << code;
    }
    virtual ~MsgServerCallback(){}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    rtc::LogMessage::LogToDebug(rtc::INFO);
    XMsgClient client;
    MsgServerCallback callback;
    const std::string server("192.168.7.27");
    int port = 9210;
    //bool autoConnect = true;
    LOG(INFO) << "begin connect to server...";
    client.Init(callback, server, port);
    LOG(INFO) << "connectted to server...";
    client.Login("test001", "123456");
    client.CreateRoom("test001", "123456", "roomid");
    client.EnterRoom("test001", "123456", "roomid");
    while (1) {
        //LOG(INFO) << "pClient->Status:" << client.Status();
        client.SndMsg("test001", "123456", "roomid", "hello world, are you ok?");
        rtc::Thread::SleepMs(3000);
    }
    LOG(INFO) << "bye bye client...";
    return 0;
}
