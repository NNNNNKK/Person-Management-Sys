#include "algorithm_hmac.h"  
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <string> 
#include "algorithm"
using std::string;  


int HmacEncode(string algorithm,  
			   string key,  
			   string input,
			   unsigned char *output, 
			   unsigned int &output_length)
{  
	transform(algorithm.begin(), algorithm.end(), algorithm.begin(), toupper);

	const EVP_MD * engine = NULL;  
	if(algorithm.compare("SHA512") == 0) 
	{ 
	   engine = EVP_sha512();  
	}  
	else if(algorithm.compare("SHA256") == 0) 
	{  
	   engine = EVP_sha256();  
	}
	else if(algorithm.compare("SHA1") == 0) 
	{  
	   engine = EVP_sha1();  
	}  
	else if(algorithm.compare("MD5") == 0) 
	{  
	   engine = EVP_md5();  
	}  
	else if(algorithm.compare("SHA224") == 0) 
	{  
	   engine = EVP_sha224();  
	}  
	else if(algorithm.compare("SHA384") == 0) 
	{  
	   engine = EVP_sha384();  
	}  
	else if(algorithm.compare("SHA") == 0) 
	{  
	   engine = EVP_sha();  
	} 
	else 
	{ 
	   return -1;  
	}  

	if(output == NULL)
	{
		return -1;
	}

	HMAC_CTX ctx;  
	HMAC_CTX_init(&ctx);  
	HMAC_Init_ex(&ctx, key.c_str(), static_cast<int>(key.length()), engine, NULL);
	HMAC_Update(&ctx, (const unsigned char *)input.c_str(), input.length());

	HMAC_Final(&ctx, output, &output_length);  
	HMAC_CTX_cleanup(&ctx);

	return 0;
}  

int Base64Encode(const unsigned char *str_in, int str_in_len, char *encode, int encode_len)
{
	int total = 0;
	EVP_ENCODE_CTX ctx;

	EVP_EncodeInit(&ctx);
	EVP_EncodeUpdate(&ctx, (unsigned char *)encode, &encode_len, (const unsigned char *)str_in, str_in_len);
	total += encode_len;
	EVP_EncodeFinal(&ctx, (unsigned char *)encode+total, &encode_len);
	total += encode_len;
	
	return total;
}