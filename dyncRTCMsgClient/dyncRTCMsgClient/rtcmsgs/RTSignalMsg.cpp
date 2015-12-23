//
//  RTSignalMsg.cpp
//  dyncRTMsgClient
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <stdio.h>
#include "RTSignalMsg.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"

std::string SIGNALMSG::ToJson()
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    
    jDoc.SetObject();
    jDoc.AddMember("stype", SIGNALMSG::_stype, jDoc.GetAllocator());
    jDoc.AddMember("scont", SIGNALMSG::_scont.c_str(), jDoc.GetAllocator());
    
    jDoc.Accept(writer);
    std::string s = sb.GetString();
    return s;
}

void SIGNALMSG::GetMsg(const std::string& str, std::string& err)
{
    if (str.empty() || str.length() == 0) {
        err.assign("bad params");
        return;
    }
    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)str.c_str()).HasParseError())
    {
        err.assign("parse error");
        return;
    }
    if(!(jsonReqDoc.HasMember("stype") && jsonReqDoc["stype"].IsInt()))
    {
        err.assign("parse stype error");
        return;
    }
    _stype = (SIGNALTYPE)jsonReqDoc["stype"].GetInt();
    if(!(jsonReqDoc.HasMember("scont") && jsonReqDoc["scont"].IsString()))
    {
        err.assign("parse scont error");
        return;
    }
    _scont = jsonReqDoc["scont"].GetString();
}