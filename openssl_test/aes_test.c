#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>

void encrypt_decrypt()
{
    const unsigned char *sec = "xll";        // 密钥
    const unsigned char *str = "hello";      // 原文
    unsigned char iv[AES_BLOCK_SIZE];        // 向量
    unsigned char cipher[AES_BLOCK_SIZE];    // 密文
    unsigned char decode[AES_BLOCK_SIZE];    // 解密后原文
    AES_KEY key;
    int len = strlen(str) + 1;
    int lengtn = 0;                         // 生成密文、原文长度

    if (len % 16 != 0)
    {
        lengtn = (len / 16 + 1) * 16;
    }
    else
    {
        lengtn = len;
    }

    // encrypt
    memset(iv, 1, sizeof(iv));
    AES_set_encrypt_key(sec, 128, &key);
    AES_cbc_encrypt(str, cipher, lengtn, &key, iv, AES_ENCRYPT);
    printf("cipher:\n %s\n", cipher);


    // decrypt
    AES_set_decrypt_key(sec, 128, &key);
    memset(iv, 1, sizeof(iv)); 
    AES_cbc_encrypt(cipher, decode, lengtn, &key, iv, AES_DECRYPT);
    decode[lengtn - 1] = '\0';
    printf("decode:\n %s\n", decode);
}

int main()
{
    encrypt_decrypt();
    return 0;
}