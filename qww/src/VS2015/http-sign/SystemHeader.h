#pragma once

#include "string"
using std::string;

/**
 * ϵͳHTTPͷ����
 */
namespace SystemHeader {
    //ǩ��Header
    const string X_CA_SIGNATURE = "x-ca-signature";
    //���в���ǩ����Header
    const string X_CA_SIGNATURE_HEADERS = "x-ca-signature-headers";
    //����ʱ���
    const string X_CA_TIMESTAMP = "x-ca-timestamp";
    //������ط�Nonce,15�����ڱ���Ψһ,����ʹ��UUID
    const string X_CA_NONCE = "x-ca-nonce";
    //APP KEY
    const string X_CA_KEY = "x-ca-key";
}