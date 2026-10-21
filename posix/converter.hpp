#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <regex>
#include <iomanip>
#include <stdexcept>
#include <cstdint>
#include "json.hpp"
#include <algorithm>
using json = nlohmann::json;

std::vector<uint8_t> b64ToBytes(const std::string& strInput) {
    std::string strB64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::map<char, int32_t> mapi32Map;
    for (int32_t i32Idx = 0; i32Idx < 64; i32Idx++) {
        mapi32Map[strB64[i32Idx]] = i32Idx;
    }
    mapi32Map['-'] = 62;
    mapi32Map['_'] = 63;

    std::vector<uint8_t> vecu8Out;
    int32_t i32Val = 0;
    int32_t i32Bits = 0;

    for (size_t szIdx = 0; szIdx < strInput.length(); szIdx++) {
        char chChar = strInput[szIdx];
        if (chChar == '=') break;
        if (mapi32Map.find(chChar) == mapi32Map.end()) continue;

        i32Val = (i32Val << 6) | mapi32Map[chChar];
        i32Bits += 6;

        if (i32Bits >= 8) {
            i32Bits -= 8;
            vecu8Out.push_back(static_cast<uint8_t>((i32Val >> i32Bits) & 0xFF));
            i32Val &= (1 << i32Bits) - 1;
        }
    }
    return vecu8Out;
}

std::string utf8Decode(const std::vector<uint8_t>& vecu8Bytes) {
    return std::string(vecu8Bytes.begin(), vecu8Bytes.end());
}

std::string urlEncode(const std::string& strInput) {
    std::ostringstream ossOut;
    ossOut.fill('0');
    ossOut << std::hex;
    for (char chChar : strInput) {
        if (isalnum(static_cast<unsigned char>(chChar)) || chChar == '-' || chChar == '_' || chChar == '.' || chChar == '~') {
            ossOut << chChar;
        }
        else {
            ossOut << std::uppercase;
            ossOut << '%' << std::setw(2) << static_cast<int32_t>(static_cast<unsigned char>(chChar));
            ossOut << std::nouppercase;
        }
    }
    return ossOut.str();
}

std::string num(const json& jsonVal) {
    if (jsonVal.is_null()) return "";
    std::string strVal = jsonVal.is_string() ? jsonVal.get<std::string>() : jsonVal.dump();
    std::regex rxNum("[\\d.]+");
    std::smatch smMatch;
    if (std::regex_search(strVal, smMatch, rxNum)) {
        return smMatch.str(0);
    }
    return "";
}

std::string first(const json& jsonVal) {
    if (jsonVal.is_null()) return "";
    if (jsonVal.is_array()) {
        if (!jsonVal.empty() && jsonVal[0].is_string()) return jsonVal[0].get<std::string>();
        return "";
    }
    if (jsonVal.is_string()) return jsonVal.get<std::string>();
    return "";
}

class CQuery {
private:
    std::vector<std::string> vecstrParams;
public:
    void add(const std::string& strKey, const std::string& strVal) {
        if (strVal.empty()) return;
        vecstrParams.push_back(urlEncode(strKey) + "=" + urlEncode(strVal));
    }

    std::string toString() const {
        std::string strRes = "";
        for (size_t szIdx = 0; szIdx < vecstrParams.size(); szIdx++) {
            strRes += vecstrParams[szIdx];
            if (szIdx < vecstrParams.size() - 1) strRes += "&";
        }
        return strRes;
    }
};

std::string buildHysteria2(const json& jsonOb, const std::string& strName) {
    json jsonS = jsonOb.value("settings", json::object());
    json jsonSs = jsonOb.value("streamSettings", json::object());
    json jsonTls = jsonSs.value("tlsSettings", json::object());
    json jsonFm = jsonSs.value("finalmask", json::object());
    json jsonQ = jsonFm.value("quicParams", json::object());
    json jsonHs = jsonSs.value("hysteriaSettings", json::object());

    CQuery objQuery;

    if (jsonTls.contains("serverName") && jsonTls["serverName"].is_string()) {
        objQuery.add("sni", jsonTls["serverName"].get<std::string>());
    }
    if (jsonTls.value("allowInsecure", false)) {
        objQuery.add("insecure", "1");
    }
    if (jsonTls.contains("pinnedPeerCertSha256")) {
        objQuery.add("pinSHA256", first(jsonTls["pinnedPeerCertSha256"]));
    }
    if (jsonTls.contains("alpn") && jsonTls["alpn"].is_array() && !jsonTls["alpn"].empty()) {
        std::string strAlpn = "";
        for (size_t szIdx = 0; szIdx < jsonTls["alpn"].size(); szIdx++) {
            strAlpn += jsonTls["alpn"][szIdx].get<std::string>();
            if (szIdx < jsonTls["alpn"].size() - 1) strAlpn += ",";
        }
        objQuery.add("alpn", strAlpn);
    }
    if (jsonTls.contains("fingerprint") && jsonTls["fingerprint"].is_string()) {
        objQuery.add("fp", jsonTls["fingerprint"].get<std::string>());
    }
    if (jsonQ.contains("brutalUp")) {
        objQuery.add("up", num(jsonQ["brutalUp"]));
    }
    if (jsonQ.contains("brutalDown")) {
        objQuery.add("down", num(jsonQ["brutalDown"]));
    }

    if (jsonFm.contains("udp") && jsonFm["udp"].is_array()) {
        for (const auto& jsonUdp : jsonFm["udp"]) {
            if (jsonUdp.value("type", "") == "salamander") {
                objQuery.add("obfs", "salamander");
                if (jsonUdp.contains("settings") && jsonUdp["settings"].contains("password") && jsonUdp["settings"]["password"].is_string()) {
                    objQuery.add("obfs-password", jsonUdp["settings"]["password"].get<std::string>());
                }
                break;
            }
        }
    }

    std::string strAuth = "";
    if (jsonHs.contains("auth") && jsonHs["auth"].is_string()) {
        strAuth = jsonHs["auth"].get<std::string>();
    }
    else if (jsonS.contains("auth") && jsonS["auth"].is_string()) {
        strAuth = jsonS["auth"].get<std::string>();
    }

    std::string strAddress = jsonS.value("address", "");
    std::string strPort = jsonS.contains("port") ? (jsonS["port"].is_number() ? std::to_string(jsonS["port"].get<int32_t>()) : jsonS["port"].get<std::string>()) : "";

    return "hysteria2://" + urlEncode(strAuth) + "@" + strAddress + ":" + strPort + "/?" + objQuery.toString() + "#" + urlEncode(strName);
}

std::string buildVless(const json& jsonOb, const std::string& strName) {
    json jsonS = jsonOb.value("settings", json::object());
    json jsonSs = jsonOb.value("streamSettings", json::object());

    std::string strAddress, strPort, strId, strFlow, strEnc;

    if (jsonS.contains("vnext") && jsonS["vnext"].is_array() && !jsonS["vnext"].empty()) {
        json jsonV = jsonS["vnext"][0];
        json jsonU = (jsonV.contains("users") && jsonV["users"].is_array() && !jsonV["users"].empty()) ? jsonV["users"][0] : json::object();
        strAddress = jsonV.value("address", "");
        strPort = jsonV.contains("port") ? (jsonV["port"].is_number() ? std::to_string(jsonV["port"].get<int32_t>()) : jsonV["port"].get<std::string>()) : "";
        strId = jsonU.value("id", "");
        strFlow = jsonU.value("flow", "");
        strEnc = jsonU.value("encryption", "");
    }
    else {
        strAddress = jsonS.value("address", "");
        strPort = jsonS.contains("port") ? (jsonS["port"].is_number() ? std::to_string(jsonS["port"].get<int32_t>()) : jsonS["port"].get<std::string>()) : "";
        strId = jsonS.value("id", "");
        strFlow = jsonS.value("flow", "");
        strEnc = jsonS.value("encryption", "");
    }

    std::string strNet = jsonSs.value("network", "tcp");
    std::string strSec = jsonSs.value("security", "none");

    CQuery objQuery;
    objQuery.add("encryption", strEnc.empty() ? "none" : strEnc);
    objQuery.add("flow", strFlow);
    objQuery.add("type", strNet);
    objQuery.add("security", strSec);

    if (strSec == "tls") {
        json jsonT = jsonSs.value("tlsSettings", json::object());
        if (jsonT.contains("serverName") && jsonT["serverName"].is_string()) objQuery.add("sni", jsonT["serverName"].get<std::string>());
        if (jsonT.contains("fingerprint") && jsonT["fingerprint"].is_string()) objQuery.add("fp", jsonT["fingerprint"].get<std::string>());
        if (jsonT.contains("alpn") && jsonT["alpn"].is_array() && !jsonT["alpn"].empty()) {
            std::string strAlpn = "";
            for (size_t szIdx = 0; szIdx < jsonT["alpn"].size(); szIdx++) {
                strAlpn += jsonT["alpn"][szIdx].get<std::string>();
                if (szIdx < jsonT["alpn"].size() - 1) strAlpn += ",";
            }
            objQuery.add("alpn", strAlpn);
        }
        if (jsonT.value("allowInsecure", false)) objQuery.add("allowInsecure", "1");
    }
    else if (strSec == "reality") {
        json jsonR = jsonSs.value("realitySettings", json::object());
        if (jsonR.contains("serverName") && jsonR["serverName"].is_string()) objQuery.add("sni", jsonR["serverName"].get<std::string>());
        if (jsonR.contains("fingerprint") && jsonR["fingerprint"].is_string()) objQuery.add("fp", jsonR["fingerprint"].get<std::string>());
        std::string strPbk = jsonR.contains("publicKey") && jsonR["publicKey"].is_string() ? jsonR["publicKey"].get<std::string>() : jsonR.value("password", "");
        objQuery.add("pbk", strPbk);
        if (jsonR.contains("shortId") && jsonR["shortId"].is_string()) objQuery.add("sid", jsonR["shortId"].get<std::string>());
        if (jsonR.contains("spiderX") && jsonR["spiderX"].is_string()) objQuery.add("spx", jsonR["spiderX"].get<std::string>());
        if (jsonR.contains("mldsa65Verify") && jsonR["mldsa65Verify"].is_string()) objQuery.add("pqv", jsonR["mldsa65Verify"].get<std::string>());
    }

    json jsonN = jsonSs.value(strNet + "Settings", json::object());

    if (strNet == "ws" || strNet == "httpupgrade" || strNet == "xhttp" || strNet == "splithttp") {
        if (jsonN.contains("path") && jsonN["path"].is_string()) objQuery.add("path", jsonN["path"].get<std::string>());
        std::string strHost = jsonN.contains("host") && jsonN["host"].is_string() ? jsonN["host"].get<std::string>() : "";
        if (strHost.empty() && jsonN.contains("headers") && jsonN["headers"].contains("Host") && jsonN["headers"]["Host"].is_string()) {
            strHost = jsonN["headers"]["Host"].get<std::string>();
        }
        objQuery.add("host", strHost);
        if (jsonN.contains("mode") && jsonN["mode"].is_string()) objQuery.add("mode", jsonN["mode"].get<std::string>());
        if (jsonN.contains("extra")) objQuery.add("extra", jsonN["extra"].dump());
    }
    else if (strNet == "grpc") {
        if (jsonN.contains("serviceName") && jsonN["serviceName"].is_string()) objQuery.add("serviceName", jsonN["serviceName"].get<std::string>());
        if (jsonN.contains("authority") && jsonN["authority"].is_string()) objQuery.add("authority", jsonN["authority"].get<std::string>());
        if (jsonN.value("multiMode", false)) objQuery.add("mode", "multi");
    }
    else if (strNet == "tcp") {
        if (jsonN.contains("header") && jsonN["header"].value("type", "") == "http") {
            objQuery.add("headerType", "http");
            json jsonRq = jsonN["header"].value("request", json::object());
            if (jsonRq.contains("path")) objQuery.add("path", first(jsonRq["path"]));
            if (jsonRq.contains("headers") && jsonRq["headers"].contains("Host")) objQuery.add("host", first(jsonRq["headers"]["Host"]));
        }
    }
    else if (strNet == "kcp" || strNet == "mkcp") {
        if (jsonN.contains("header") && jsonN["header"].contains("type") && jsonN["header"]["type"].is_string()) {
            objQuery.add("headerType", jsonN["header"]["type"].get<std::string>());
        }
        if (jsonN.contains("seed") && jsonN["seed"].is_string()) {
            objQuery.add("seed", jsonN["seed"].get<std::string>());
        }
    }

    return "vless://" + urlEncode(strId) + "@" + strAddress + ":" + strPort + "?" + objQuery.toString() + "#" + urlEncode(strName);
}

std::string outboundToLink(const json& jsonOb, const std::string& strName) {
    std::string strProtocol = jsonOb.value("protocol", "");
    if (strProtocol == "hysteria" || strProtocol == "hysteria2") return buildHysteria2(jsonOb, strName);
    if (strProtocol == "vless") return buildVless(jsonOb, strName);
    throw std::runtime_error("protocol not supported: " + strProtocol);
}

json findProxy(const json& jsonConfig) {
    json jsonList = jsonConfig.contains("outbounds") ? jsonConfig["outbounds"] : (jsonConfig.contains("protocol") ? json::array({ jsonConfig }) : json::array());
    for (const auto& jsonItem : jsonList) {
        if (jsonItem.value("tag", "") == "proxy") return jsonItem;
    }
    for (const auto& jsonItem : jsonList) {
        std::string strProtocol = jsonItem.value("protocol", "");
        if (strProtocol == "vless" || strProtocol == "hysteria" || strProtocol == "hysteria2") return jsonItem;
    }
    throw std::runtime_error("proxy outbound not found");
}

std::string trimStr(const std::string& strInput) {
    size_t szStart = strInput.find_first_not_of(" \t\r\n");
    if (szStart == std::string::npos) return "";
    size_t szEnd = strInput.find_last_not_of(" \t\r\n");
    return strInput.substr(szStart, szEnd - szStart + 1);
}

std::string decodeLink(std::string strText, bool bIsRawJson) {
    strText = trimStr(strText);
    std::string strPrefix = "throne://add/";
    json jsonOuter;

    if (bIsRawJson) {
        jsonOuter = json::parse(strText);
    }
    else if (strText.find(strPrefix) == 0) {
        std::string strB64 = strText.substr(strPrefix.length());
        std::vector<uint8_t> vecu8Decoded = b64ToBytes(strB64);
        std::string strJson = utf8Decode(vecu8Decoded);
        jsonOuter = json::parse(strJson);
    }
    else {
        jsonOuter = json::parse(strText);
    }

    json jsonConfig = jsonOuter;
    if (jsonOuter.contains("config")) {
        if (jsonOuter["config"].is_string()) {
            jsonConfig = json::parse(jsonOuter["config"].get<std::string>());
        }
        else {
            jsonConfig = jsonOuter["config"];
        }
    }

    std::string strName = "proxy";
    if (jsonOuter.contains("name") && jsonOuter["name"].is_string()) {
        strName = jsonOuter["name"].get<std::string>();
    }
    else if (jsonConfig.contains("remarks") && jsonConfig["remarks"].is_string()) {
        strName = jsonConfig["remarks"].get<std::string>();
    }

    return outboundToLink(findProxy(jsonConfig), strName);
}

std::string convertAll(std::string strText, bool bIsRawJson) {
    strText.erase(std::remove(strText.begin(), strText.end(), '\n'), strText.end());
    strText.erase(std::remove(strText.begin(), strText.end(), '\r'), strText.end());

    strText = trimStr(strText);
    std::vector<std::string> vecstrParts;

    if (strText.find("throne://") == 0) {
        std::regex rxWhitespace("\\s+");
        std::sregex_token_iterator itStart(strText.begin(), strText.end(), rxWhitespace, -1);
        std::sregex_token_iterator itEnd;
        for (; itStart != itEnd; ++itStart) {
            if (!itStart->str().empty()) vecstrParts.push_back(itStart->str());
        }
    }
    else {
        vecstrParts.push_back(strText);
    }

    std::string strOut = "";
    for (size_t szIdx = 0; szIdx < vecstrParts.size(); szIdx++) {
        try {
            strOut += decodeLink(vecstrParts[szIdx], bIsRawJson);
        }
        catch (const std::exception& objException) {
            strOut += "ERROR: ";
            strOut += objException.what();
        }
        if (szIdx < vecstrParts.size() - 1) {
            strOut += "\n";
        }
    }
    return strOut;
}