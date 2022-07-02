#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

// 密钥对生成
void rsa_pri_pub_generate()
{
    BIGNUM *big;
    FILE *fp;
    RSA *rsa;

    do
    {
        /* int RSA_generate_key_ex(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb); */
        if ((rsa = RSA_new()) == NULL || (big = BN_new()) == NULL)
        {
            printf("RSA_new or BN_new err\n");
            return;
        }
        BN_set_word(big, 12345);
        if (0 == RSA_generate_key_ex(rsa, 1024, big, NULL))
        {
            printf("RSA_generate_key_ex failed...\n");
            return;
        }

        // 写入文件
        fp = fopen("public.pem", "w");
        PEM_write_RSAPublicKey(fp, rsa);
        fclose(fp);

        fp = fopen("private.pem", "w");
        PEM_write_RSAPrivateKey(fp, rsa, NULL, NULL, 0, NULL, NULL);
    } while (0);

    RSA_free(rsa);
    BN_free(big);
    fclose(fp);
}

// 公钥加密、私钥解密
void encrypt_decrypt()
{
    RSA *pubKey;
    RSA *priKey;
    FILE *fr;
    FILE *fw;
    int size;
    char *cipher;
    char *decode;
    const char *str = "hello"; // 原文

    // 从磁盘中读取公私钥文件
    fr = fopen("public.pem", "r");
    if (fr == NULL)
    {
        printf("open public.pem failed\n");
        return;
    }
    pubKey = RSA_new();
    PEM_read_RSAPublicKey(fr, &pubKey, NULL, NULL);
    fflush(fr);
    fclose(fr);

    fw = fopen("private.pem", "r");
    if (fw == NULL)
    {
        printf("open private.pem failed\n");
        return;
    }
    priKey = RSA_new();
    PEM_read_RSAPrivateKey(fw, &priKey, NULL, NULL);
    fflush(fw);
    fclose(fw);
    
    size = RSA_size(pubKey);
    cipher = (char *)malloc(size * sizeof(char));
    if (cipher == NULL)
    {
        printf("malloc failed\n");
        return;
    }
    // 加密
    if (RSA_public_encrypt(strlen(str), str, cipher, pubKey, RSA_PKCS1_PADDING) == 0)
    {
        printf("encrypt failed\n");
        return;
    }

    printf("cipher: %s\n", cipher);

    // 解密
    size = RSA_size(priKey);
    decode = (char *)malloc(size * sizeof(char));
    if (decode == NULL)
    {
        printf("malloc failed\n");
        return;
    }
    if (RSA_private_decrypt(strlen(cipher), cipher, decode, priKey, RSA_PKCS1_PADDING) == 0)
    {
        printf("decrypt failed\n");
        return;
    }
    printf("decode: %s\n", decode);

    RSA_free(pubKey);
    RSA_free(priKey);
    free(cipher);
    free(decode);
}

int main()
{
    // rsa_pri_pub_generate();
    encrypt_decrypt();
    return 0;
}