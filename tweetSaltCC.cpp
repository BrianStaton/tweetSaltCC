#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "tweetnacl.h"
#include "naclSupport.h"

uint8_t plainText[512];
uint8_t cypherText[512];
uint8_t decypherText[512];

uint8_t client_pk[crypto_box_PUBLICKEYBYTES];
uint8_t client_sk[crypto_box_SECRETKEYBYTES];

uint8_t server_pk[crypto_box_PUBLICKEYBYTES];
uint8_t server_sk[crypto_box_SECRETKEYBYTES];

uint8_t nonce[crypto_box_NONCEBYTES];

void printHex(const uint8_t *data, size_t n, const char *msg)
{
    size_t i;

    if (msg)
    {
        printf("%s(%ld)", msg, n);
    }
    for(i=0; i<n; i++)
    {
        if (0 == (i % 16))
        {
            printf("\n");
        }
        printf("%02X ", data[i]);
    }
    printf("\n");
}

int main()
{
    crypto_box_keypair(client_pk, client_sk);
    crypto_box_keypair(server_pk, server_sk);
    randombytes(nonce, crypto_box_NONCEBYTES);

    printHex(client_pk, crypto_box_PUBLICKEYBYTES, "Client Public Key");
    printHex(client_sk, crypto_box_SECRETKEYBYTES, "Client Secret Key");
    printHex(server_pk, crypto_box_PUBLICKEYBYTES, "Server Public Key");
    printHex(server_sk, crypto_box_SECRETKEYBYTES, "Server Secret Key");
    printHex(nonce, crypto_box_NONCEBYTES, "Nonce");

    memset(plainText, 0, sizeof(plainText));
    strcpy((char *)(&plainText[crypto_box_ZEROBYTES]), "Hello World");
    size_t msgLen = strlen((char *)(&plainText[crypto_box_ZEROBYTES])) + 1;
    size_t paddedMsgLen = msgLen + crypto_box_ZEROBYTES;

    printHex(plainText, paddedMsgLen, "Padded Plain Text");

    crypto_box(cypherText, plainText, paddedMsgLen, nonce, server_pk, client_sk);

    printHex(cypherText, paddedMsgLen, "Cypher Text");

    // crypto_box_BOXZEROBYTES
    int result = crypto_box_open(decypherText, cypherText, paddedMsgLen, nonce, client_pk, server_sk);
    printHex(decypherText, paddedMsgLen, "Decypher Text");
    if (0 == result)
    {
        printf("Decypher Msg: %s\n", (char *)(&decypherText[crypto_box_ZEROBYTES]));
    }
    else
    {
        printf("Fail\n");
    }
    return 0;
}
