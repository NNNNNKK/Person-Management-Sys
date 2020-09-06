#pragma once
#include <string>
using std::string;
/**
 * hmac加密
 *
 * @param algorithm            指定所用的加密算法
 * @param key                  加密用的key
 * @param input				   需要加密的数据
 * @param output               加密后的数据
 * @param output_length        加密后的数据长度
 * @return 0 成功；-1失败
 */
int HmacEncode(string algorithm,  
			   string key,  
			   string input,
			   unsigned char *output,
			   unsigned int &output_length);

int Base64Encode(const unsigned char *str,int str_len,char *encode,int encode_len);