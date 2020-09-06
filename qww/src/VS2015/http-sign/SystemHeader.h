#pragma once

#include "string"
using std::string;

/**
 * 系统HTTP头常量
 */
namespace SystemHeader {
    //签名Header
    const string X_CA_SIGNATURE = "x-ca-signature";
    //所有参与签名的Header
    const string X_CA_SIGNATURE_HEADERS = "x-ca-signature-headers";
    //请求时间戳
    const string X_CA_TIMESTAMP = "x-ca-timestamp";
    //请求放重放Nonce,15分钟内保持唯一,建议使用UUID
    const string X_CA_NONCE = "x-ca-nonce";
    //APP KEY
    const string X_CA_KEY = "x-ca-key";
}