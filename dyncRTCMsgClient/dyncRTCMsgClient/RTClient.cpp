#include "RTClient.h"
#include "DyncTypes.h"
#include "webrtc/base/base64.h"
#include "webrtc/base/md5digest.h"
#include "webrtc/base/timeutils.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

const int		kRequestBufferSizeInBytes = 2048;
#define FREE_PTR(ptr) \
    if (NULL != (ptr)) {\
        free (ptr);\
        (ptr) = NULL;\
	    }

#define DELETE_PTR(ptr) \
    if (NULL != (ptr)) {\
        delete (ptr);\
        (ptr) = NULL;\
	    }
static void writeShort(char** pptr, unsigned short anInt)
{
	**pptr = (char)(anInt / 256);
	(*pptr)++;
	**pptr = (char)(anInt % 256);
	(*pptr)++;
}

static unsigned short readShort(char** pptr)
{
	char* ptr = *pptr;
	unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
	*pptr += 2;
	return len;
}

RTClient::RTClient()
	: SynClient(10)
	, m_nTime2disconnect(0)
	, m_pTcpClient(NULL)
	, m_nBufOffset(0)
	, m_bConnected(false)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
	m_nParseBufLen = kRequestBufferSizeInBytes;
	m_pParseBuf = new char[m_nParseBufLen];

	m_pTcpClient = XTcpClient::Create(*this);
}

RTClient::~RTClient()
{
	m_pTcpClient->Disconnect();
	DELETE_PTR(m_pTcpClient);

	delete[] m_pBuffer;
	delete[] m_pParseBuf;
}

int RTClient::ConnectStatus()
{
	if (m_pTcpClient == NULL)
		return 0;

	return m_pTcpClient->Status();
}

void RTClient::Connect(const std::string&strSvrAddr, int nSvrPort, const std::string&developerId, const std::string&strToken, const std::string&strAESKey, const std::string&strAppId)
{
	{// Init data.
		m_strSvrAddr = strSvrAddr;
		m_nSvrPort = nSvrPort;
		m_strDeveloperId = developerId;
		m_strAppId = strAppId;
		m_strToken = strToken;
	}
	
	SynClient::Clear();
	SynClient::SetEncrypt(strToken, strAESKey, strAppId);

	DELETE_PTR(m_pTcpClient);
	m_pTcpClient = XTcpClient::Create(*this);
	m_pTcpClient->Connect(strSvrAddr, nSvrPort, true);
}

void RTClient::Disconnect()
{
	SynClient::Disconnect();

	if (m_nTime2disconnect == 0)
		m_nTime2disconnect = rtc::Time() + 500;
}

void RTClient::OnConnected(int code, const std::string&strUserData)
{
	if (code == 200)
	{
		m_bConnected = true;
	}

	if (strUserData.length() > 0) {
		rapidjson::Document		jsonReqDoc;
		if (!jsonReqDoc.ParseInsitu<0>((char*)strUserData.c_str()).HasParseError())
		{
			m_strDyncId = jsonReqDoc["DyncId"].GetString();
			m_strServerId = jsonReqDoc["ServerId"].GetString();
		}
	}
	
	this->OnRtcConnect(code, m_strDyncId, m_strServerId);
}

void RTClient::OnMessageSend(const char*pMsg, int nLen)
{
	if (m_pTcpClient) {
		char buffer[3];
		char* pptr = buffer;
		pptr[0] = '$';
		pptr++;
		writeShort(&pptr, nLen + 3);
		m_pTcpClient->SendMessageX(buffer, 3);
		m_pTcpClient->SendMessageX(pMsg, nLen);
	}
}
void RTClient::OnMessageRecved(int cmd, MapParams& params, const std::string& strContent)
{
	switch (cmd) {
	case DC_MESSAGE: {
		std::string&strFrom = params["From"];
		OnRtcMessage(strFrom, strContent);
	}
		break;
	case DC_PUBLISH:
	{
		std::string&strResult = params["Result"];
		char buff[128] = { 0 };
		sprintf(buff, "%s@%s", params["ChanId"].c_str(), params["SvrId"].c_str());
		std::string strBaseId = rtc::Base64::Encode(buff);
		OnRtcPublish(strResult, strBaseId);
	}
		break;
	case DC_UNPUBLISH:
	{
		std::string&strResult = params["Result"];
		OnRtcUnpublish(strResult, params["ChanId"]);
	}
		break;
	case DC_SUBSCRIBE:
	{
		std::string&strResult = params["Result"];
		char buff[128] = { 0 };
		sprintf(buff, "%s@%s", params["ChanId"].c_str(), params["SvrId"].c_str());
		std::string strBaseId = rtc::Base64::Encode(buff);
		OnRtcSubscribe(strResult, strBaseId, rtc::Base64::Encode(params["SubId"]), strContent);
	}
		break;
	case DC_UNSUBSCRIBE:
	{
		std::string&strResult = params["Result"];
		OnRtcUnsubscribe(strResult, params["ChanId"]);
	}
		break;
	case DC_SDP_INFO:
	{
		char buff[128] = { 0 };
		sprintf(buff, "%s@%s", params["ChanId"].c_str(), params["SvrId"].c_str());
		std::string strBaseId = rtc::Base64::Encode(buff);
		OnRtcSdpInfo(strBaseId, strContent);
	}
		break;
	};
	
}
void RTClient::OnDisconnect()
{
	SynClient::Clear();
	this->OnRtcDisconnect();
}

void RTClient::Message(const std::string&strTo, const std::string&strToSvrId, const std::string&strContent)
{
	MapParams param;
	param["From"] = m_strDyncId;
	param["To"] = strTo;
	param["ToSvr"] = strToSvrId;

	SynClient::SendMessageX(DC_MESSAGE, true, param, strContent);
}

void RTClient::Publish(const std::string&strType, int width, int height, int aBitrate, int vBitrate)
{
	MapParams param;
	rapidjson::Document		jsonDoc;
	rapidjson::StringBuffer jsonStr;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
	jsonDoc.SetObject();
	jsonDoc.AddMember("Type", strType.c_str(), jsonDoc.GetAllocator());
	jsonDoc.AddMember("Width", width, jsonDoc.GetAllocator());
	jsonDoc.AddMember("Height", height, jsonDoc.GetAllocator());
	jsonDoc.AddMember("AudioBitrate", aBitrate, jsonDoc.GetAllocator());
	jsonDoc.AddMember("VideoBitrate", vBitrate, jsonDoc.GetAllocator());

	jsonDoc.Accept(jsonWriter);
	SynClient::SendMessageX(DC_PUBLISH, true, param, jsonStr.GetString());
}

void RTClient::Unpublish(const std::string&strPublishId)
{
	MapParams param;
	rapidjson::Document		jsonDoc;
	rapidjson::StringBuffer jsonStr;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
	jsonDoc.SetObject();
	std::string strChanId;
	{
		std::string strDecode = rtc::Base64::Decode(strPublishId, rtc::Base64::DO_STRICT);
		int index = strDecode.find("@");
		//ASSERT(index > 0);
		strChanId = strDecode.substr(0, index);
	}
	jsonDoc.AddMember("PublishId", strChanId.c_str(), jsonDoc.GetAllocator());

	jsonDoc.Accept(jsonWriter);
	SynClient::SendMessageX(DC_UNPUBLISH, true, param, jsonStr.GetString());
}

void RTClient::Subscribe(const std::string&strPublishId, const std::string&strSvrerId, int aBitrate, int vBitrate)
{
	MapParams param;
	rapidjson::Document		jsonDoc;
	rapidjson::StringBuffer jsonStr;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
	jsonDoc.SetObject();
	std::string strChanId;
	std::string strSvrId;
	{
		std::string strDecode = rtc::Base64::Decode(strPublishId, rtc::Base64::DO_STRICT);
		int index = strDecode.find("@");
		//ASSERT(index > 0);
		strChanId = strDecode.substr(0, index);
		strSvrId = strDecode.substr(index + 1);
	}
	jsonDoc.AddMember("ChanId", strChanId.c_str(), jsonDoc.GetAllocator());
	jsonDoc.AddMember("ChanSvr", strSvrId.c_str(), jsonDoc.GetAllocator());
	jsonDoc.AddMember("AudioBitrate", aBitrate, jsonDoc.GetAllocator());
	jsonDoc.AddMember("VideoBitrate", vBitrate, jsonDoc.GetAllocator());

	jsonDoc.Accept(jsonWriter);
	SynClient::SendMessageX(DC_SUBSCRIBE, true, param, jsonStr.GetString());
}

void RTClient::Unsubscribe(const std::string&strSubscribeId)
{
	MapParams param;
	rapidjson::Document		jsonDoc;
	rapidjson::StringBuffer jsonStr;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
	jsonDoc.SetObject();
	std::string strChanId;
	{
		std::string strDecode = rtc::Base64::Decode(strSubscribeId, rtc::Base64::DO_STRICT);
		int index = strDecode.find("@");
		//ASSERT(index > 0);
		strChanId = strDecode.substr(0, index);
	}
	jsonDoc.AddMember("ChanId", strChanId.c_str(), jsonDoc.GetAllocator());

	jsonDoc.Accept(jsonWriter);
	SynClient::SendMessageX(DC_UNSUBSCRIBE, true, param, jsonStr.GetString());
}

void RTClient::SendSdpInfo(const std::string&strChannelId, const std::string&strJsep)
{
	MapParams param;
	std::string strChanId;
	std::string strSvrId;
	{
		std::string strDecode = rtc::Base64::Decode(strChannelId, rtc::Base64::DO_STRICT);
		int index = strDecode.find("@");
		//ASSERT(index > 0);
		strChanId = strDecode.substr(0, index);
		strSvrId = strDecode.substr(index + 1);
	}
	param["ChanId"] = strChanId;
	param["SvrId"] = strSvrId;

	SynClient::SendMessageX(DC_SDP_INFO, true, param, strJsep);
}

void RTClient::OnServerConnected()
{
	rapidjson::Document		jsonDoc;
	rapidjson::StringBuffer jsonStr;
	rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
	jsonDoc.SetObject();
	jsonDoc.AddMember("DeveloperId", m_strDeveloperId.c_str(), jsonDoc.GetAllocator());
	jsonDoc.AddMember("AppId", m_strAppId.c_str(), jsonDoc.GetAllocator());
	jsonDoc.AddMember("DevType", "windows", jsonDoc.GetAllocator());

	jsonDoc.Accept(jsonWriter);

	SynClient::Connect(m_strToken, jsonStr.GetString());
}

void RTClient::OnServerDisconnect()
{
	SynClient::Clear();
}

void RTClient::OnServerConnectionFailure()
{

}

void RTClient::OnTick()
{
	SynClient::DoTick();

	if (m_nTime2disconnect > 0 && m_nTime2disconnect >= rtc::Time()) {
		if (m_pTcpClient)
		{
			m_pTcpClient->Disconnect();
		}
		m_nTime2disconnect = 0;
	}
}

void RTClient::OnMessageSent(int err)
{

}

void RTClient::OnMessageRecv(const char*data, int size)
{
	{//* 1,将接收到的数据放入缓存中
		while ((m_nBufOffset + size) > m_nBufLen)
		{
			int newLen = m_nBufLen + kRequestBufferSizeInBytes;
			if (size > kRequestBufferSizeInBytes)
				newLen = m_nBufLen + size;
			char* temp = new char[newLen];
			if (temp == NULL)
				continue;
			memcpy(temp, m_pBuffer, m_nBufLen);
			delete[] m_pBuffer;
			m_pBuffer = temp;
			m_nBufLen = newLen;
		}

		memcpy(m_pBuffer + m_nBufOffset, data, size);
		m_nBufOffset += size;
	}

	while (m_nBufOffset > 3)
	{//* 2,解压包
		int parsed = 0;
		if (m_pBuffer[0] != '$')
		{// Hase error!
			parsed = m_nBufOffset;
		}
		else
		{
			char*pptr = m_pBuffer + 1;
			int packLen = readShort(&pptr);
			if (packLen <= m_nBufOffset)
			{
				ParseMessage(pptr, packLen - 3);
				parsed = packLen;
			}
			else
			{
				break;
			}
		}

		if (parsed > 0)
		{
			m_nBufOffset -= parsed;
			if (m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
			}
		}
	}
}

void RTClient::ParseMessage(const char*message, int nLen)
{
	if (nLen >= m_nParseBufLen)
	{
		m_nParseBufLen = nLen + 1;
		delete[] m_pParseBuf;
		m_pParseBuf = new char[m_nParseBufLen];
	}
	memcpy(m_pParseBuf, message, nLen);
	m_pParseBuf[nLen] = '\0';
	SynClient::RecvMessageX(m_pParseBuf, nLen);
}