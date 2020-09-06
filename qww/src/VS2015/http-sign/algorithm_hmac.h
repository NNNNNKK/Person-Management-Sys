#pragma once
#include <string>
using std::string;
/**
 * hmac����
 *
 * @param algorithm            ָ�����õļ����㷨
 * @param key                  �����õ�key
 * @param input				   ��Ҫ���ܵ�����
 * @param output               ���ܺ������
 * @param output_length        ���ܺ�����ݳ���
 * @return 0 �ɹ���-1ʧ��
 */
int HmacEncode(string algorithm,  
			   string key,  
			   string input,
			   unsigned char *output,
			   unsigned int &output_length);

int Base64Encode(const unsigned char *str,int str_len,char *encode,int encode_len);