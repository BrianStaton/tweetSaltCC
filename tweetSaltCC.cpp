#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "tweetnacl.h"
#include "naclSupport.h"

#define NUMBER_OF_ZERO_BYTES    (crypto_box_ZEROBYTES)

uint8_t plainText[512];
uint8_t cypherText[512];
uint8_t decypherText[512];

uint8_t client_pk[crypto_box_PUBLICKEYBYTES];
uint8_t client_sk[crypto_box_SECRETKEYBYTES];

uint8_t server_pk[crypto_box_PUBLICKEYBYTES];
uint8_t server_sk[crypto_box_SECRETKEYBYTES];

uint8_t nonce[crypto_box_NONCEBYTES];

long myclock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

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
    long t1, t2;

    // Create key pairs for the client and the server
    crypto_box_keypair(client_pk, client_sk);
    crypto_box_keypair(server_pk, server_sk);

    // Create a random NONCE
    randombytes(nonce, crypto_box_NONCEBYTES);

    printHex(client_pk, crypto_box_PUBLICKEYBYTES, "Client Public Key");
    printHex(client_sk, crypto_box_SECRETKEYBYTES, "Client Secret Key");
    printHex(server_pk, crypto_box_PUBLICKEYBYTES, "Server Public Key");
    printHex(server_sk, crypto_box_SECRETKEYBYTES, "Server Secret Key");
    printHex(nonce, crypto_box_NONCEBYTES, "Nonce");

    // The plain text message MUST start with 32 bytes of zeros.
    // Put the real message after these zero bytes
    memset(plainText, 0, sizeof(plainText));
    strcpy((char *)(&plainText[NUMBER_OF_ZERO_BYTES]), "Hello World");
    size_t msgLen = strlen((char *)(&plainText[NUMBER_OF_ZERO_BYTES])) + 1;     // Add 1 because we want the terminating null to be part of the message
    size_t paddedMsgLen = msgLen + NUMBER_OF_ZERO_BYTES;

    // Now we have the plainText message buffer prepared with the leading zero bytes
    // followed by real message data.
    printHex(plainText, paddedMsgLen, "Padded Plain Text");

    t1 = myclock();
    crypto_box  (   cypherText      // Place to hold the encryption output
                    , plainText     // Data buffer to encrypt
                    , paddedMsgLen  // Total length of the data buffer including the leading zero bytes
                    , nonce         // Random nonce.  The recipient needs this nonce data too
                    , server_pk     // My private key
                    , client_sk     // Recipient's public key
                );
    t2 = myclock();

    printf("Encrypt time: %ld us\n", (t2-t1));

    // You'll notice that the cypherText buffer starts with 16 bytes of zeros.
    // Don't think you can therefore shorten the input buffer by 16 and eliminate these zeros - you can't.
    // Now, you don't necessarily have to transmit these 16 bytes of zero to the receiver; however if you don't
    // send them then the receiver will have to synthesize them into the data buffer fed
    // to the decryption routine.
    printHex(cypherText, paddedMsgLen, "Cypher Text");

    // Now decrypt the buffer
    t1 = myclock();
    int result = crypto_box_open    (   decypherText        // Place to hold the output
                                        , cypherText        // Data buffer to decrypt
                                        , paddedMsgLen      // Total length of the data buffer
                                        , nonce             // Random nonce used for the encryption
                                        , client_pk         // Transmitter's public key
                                        , server_sk         // My secret key
                                    );
    t2 = myclock();

    printHex(decypherText, paddedMsgLen, "Decypher Text");
    if (0 == result)
    {
        printf("Decypher Msg: %s\nDecypher Time = %ld us\n"
                , (char *)(&decypherText[NUMBER_OF_ZERO_BYTES])
                , (t2 - t1)
                );
    }
    else
    {
        printf("Fail\n");
    }
    return 0;
}
