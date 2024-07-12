#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

void print_hash(unsigned char *hash, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int main() {
    // String de entrada
    const char *input = "Texto para ser hashado";
    
    // Buffer para armazenar o hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length;
    
    // Contexto de mensagem
    EVP_MD_CTX *mdctx;
    if((mdctx = EVP_MD_CTX_new()) == NULL) {
        printf("Erro ao criar contexto de mensagem.\n");
        return 1;
    }
    
    // Calcular SHA-256
    if(1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        printf("Erro ao inicializar digest.\n");
        return 1;
    }
    if(1 != EVP_DigestUpdate(mdctx, input, strlen(input))) {
        printf("Erro ao atualizar digest.\n");
        return 1;
    }
    if(1 != EVP_DigestFinal_ex(mdctx, hash, &hash_length)) {
        printf("Erro ao finalizar digest.\n");
        return 1;
    }
    
    // Limpar contexto
    EVP_MD_CTX_free(mdctx);
    
    // Imprimir o hash
    printf("Hash SHA-256: ");
    print_hash(hash, hash_length);
    
    return 0;
}
