//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "XMsgClient.h"
#include "webrtc/base/logging.h"
#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: m_pClient(NULL)
, m_pMsgProcesser(NULL)
{
    
}

XMsgClient::~XMsgClient()
{
    
}

int XMsgClient::Init(XMsgCallback& cb, const std::string& server, int port, bool bAutoConnect)
{
    if (!m_pClient) {
        m_pClient = XTcpClient::Create(*this);
    }
    if (!m_pClient) {
        return -1;
    }
    m_pClient->Connect(server, port, bAutoConnect);
    m_pMsgProcesser = new XMsgProcesser(cb);
    return 0;
}

int XMsgClient::Unin()
{
    if (m_pMsgProcesser) {
        delete m_pMsgProcesser;
        m_pMsgProcesser = NULL;
    }
    m_pClient->Disconnect();
    if (m_pClient) {
        delete m_pClient;
        m_pClient = NULL;
    }
    return 0;
}

int XMsgClient::Login(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, userid, pass);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::SndMsg(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, "a", msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::GetMsg(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeGetMsg(outstr, userid, pass);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::Logout(const std::string& userid, const std::string& pass)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, userid, pass);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::CreateRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, "", "", MEETCMD::create, 0, 0, 0);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::EnterRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, "", "", MEETCMD::enter, 0, 0, 0);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::LeaveRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, "", "", MEETCMD::leave, 0, 0, 0);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::DestroyRoom(const std::string& userid, const std::string& pass, const std::string& roomid)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, "", "", MEETCMD::destroy, 0, 0, 0);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

int XMsgClient::SndMsgTo(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        std::string tousers;
        m_pMsgProcesser->GetMemberToJson(ulist, tousers);
        m_pMsgProcesser->EncodeSndMsg(outstr, userid, "tokenfromhttpserver", roomid, tousers, msg, MEETCMD::dcomm, DCOMMACTION::msend, SENDTAGS::talk, SENDTYPE::msg);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}

bool XMsgClient::RefreshTime()
{
    uint32 now = rtc::Time();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        KeepAlive();
        return true;
    } else {
        return false;
    }
}

////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

int XMsgClient::KeepAlive()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid, pass, roomid, to, msg, cmd, action, tags, type
        m_pMsgProcesser->EncodeKeepAlive(outstr);
    } else {
        return -1;
    }
    
    char* ptr = new char[outstr.length()+5];
    sprintf(ptr, "$%4d%s", (int)outstr.length(), outstr.c_str());
    if (m_pClient) {
        m_pClient->SendMessageX(ptr, (int)strlen(ptr));
    } else {
        return -1;
    }
    return 0;
}


//////////////////////////////////////////////////
//////////////MsgClientCallback////////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnServerDisconnect()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnServerConnectionFailure()
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void XMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    RefreshTime();
}

void XMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    //LOG(INFO) << __FUNCTION__ << " was called nLen:" << nLen << ",pData:" << pData;
    int parsed = 0;
    int ll = 0;
    while (parsed < nLen)
    {
        const char* pMsg = pData + parsed;
        int offset = 0;
        if (*(pMsg+offset) == '$') {
            offset += 1;
            char l[4] = {0};
            memcpy(l, pMsg+offset, 4);
            offset += 4;
            ll = (int)strtol(l, NULL, 10);
            if (ll>0 && ll <= nLen) {
                int nlen = m_pMsgProcesser->DecodeRecvData((char *)(pMsg+offset), ll);
                if (nlen == -1) {
                    break;
                } else { // nlen == 0
                    assert(nlen == ll);
                    offset += ll;
                    parsed += offset;
                }
            } else { // ll>0 && ll <= nLen
                LOG(LS_ERROR) << "Get Msg Len Error!!!";
            }
        }
    }
}
