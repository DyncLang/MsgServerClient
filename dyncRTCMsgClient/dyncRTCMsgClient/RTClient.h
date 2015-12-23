#ifndef __RTC_CLIENT_H__
#define __RTC_CLIENT_H__
#include "XTcpClient.h"
#include "SynClient.h"
#include <map>
/*
	Resources: Turn  Mcu
*/

class RTClient : public SynClient, public XTcpClientCallback
{
public:
	RTClient();
	virtual ~RTClient();

public:
	int		ConnectStatus();

public:
	void	Connect(const std::string&strSvrAddr, int nSvrPort, const std::string&strDeveloperId, const std::string&strToken, const std::string&strAESKey, const std::string&strAppId);
	void	Publish(const std::string&strType, int width, int height, int aBitrate, int vBitrate);
	void	Unpublish(const std::string&strPublishId);
	void	Subscribe(const std::string&strPublishId, const std::string&strSvrId, int aBitrate, int vBitrate);
	void	Unsubscribe(const std::string&strSubscribeId);
	void	Message(const std::string&strTo, const std::string&strToSvrId, const std::string&strContent);
	void	SendSdpInfo(const std::string&strChanId, const std::string&strJsep);
	void	Disconnect();

public:
	//* For SynClient
	virtual void OnConnected(int code, const std::string&strUserData);
	virtual void OnMessageSend(const char*pMsg, int nLen);
	virtual void OnMessageRecved(int cmd, MapParams& params, const std::string& strContent);
	virtual void OnDisconnect();

public:
	//* For XTcpClientCallback
	virtual void OnServerConnected();
	virtual void OnServerDisconnect();
	virtual void OnServerConnectionFailure();
	virtual void OnTick();
	virtual void OnMessageSent(int err);
	virtual void OnMessageRecv(const char*pData, int nLen);

public:
	virtual void OnRtcConnect(int code, const std::string& strDyncId, const std::string& strServerId) = 0;
	virtual void OnRtcMessage(const std::string&strFromId, const std::string&strJsep) = 0;
	virtual void OnRtcPublish(const std::string&strResult, const std::string&strChannelId/*Base64(pubId@svrId)*/) = 0;
	virtual void OnRtcUnpublish(const std::string&strResult, const std::string&strChannelId) = 0;
	virtual void OnRtcSubscribe(const std::string&strResult, const std::string&strChannelId/*Base64(subId@svrId)*/, const std::string&strSubscribeId, const std::string&strJsep) = 0;
	virtual void OnRtcUnsubscribe(const std::string&strResult, const std::string&strChannelId) = 0;
	virtual void OnRtcSdpInfo(const std::string&strChannelId, const std::string&strJsep) = 0;
	virtual void OnRtcDisconnect() = 0;

protected:
	void ParseMessage(const char*message, int nLen);

private:
	XTcpClient				*m_pTcpClient;
	std::string				m_strSvrAddr;
	int						m_nSvrPort;
	bool					m_bConnected;
	uint32_t				m_nTime2disconnect;
	std::string				m_strDeveloperId;
	std::string				m_strAppId;
	std::string				m_strToken;
	std::string				m_strDyncId;
	std::string				m_strServerId;

	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
	char			*m_pParseBuf;
	int				m_nParseBufLen;
};

#endif	// __RTC_CLIENT_H__
