
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

void print_hash(unsigned char hash[SHA256_DIGEST_LENGTH]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int main() {
    // String de entrada
    const char *input = "Texto para ser hashado";
    
    // Buffer para armazenar o hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Calcular SHA-256
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);
    
    // Imprimir o hash
    printf("Hash SHA-256: ");
    print_hash(hash);
    
    return 0;
}
