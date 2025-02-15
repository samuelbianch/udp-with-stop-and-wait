#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define PORT 8080
#define PACKET_SIZE 1024
#define TIMEOUT 2

// Estrutura do pacote
typedef struct {
    int seq_num;
    int data_size;
    char data[PACKET_SIZE];
} Packet;


// Cliente UDP
void client(const char *filename, const char *server_ip) {
    time_t temp_ini = time(NULL);
    int sockfd;
    struct sockaddr_in server_addr;
    printf("\nFilename: %s", filename);
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    Packet packet;
    int seq_num = 0, bytes_read;
    socklen_t addr_len = sizeof(server_addr);
    struct timeval timeout = {TIMEOUT, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while ((bytes_read = fread(packet.data, 1, PACKET_SIZE, file)) > 0) {
        packet.seq_num = seq_num;
        packet.data_size = bytes_read;
        int ack;

        do {
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, addr_len);
            printf("Enviado pacote %d\n", seq_num);
            
            if (recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&server_addr, &addr_len) < 0) {
                printf("Timeout, retransmitindo pacote %d\n", seq_num);
            }
        } while (ack != seq_num);
        seq_num++;
    }

    // Enviar um pacote especial para indicar o fim da transmissão
    packet.seq_num = seq_num;
    packet.data_size = 0;
    sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, addr_len);
    
    fclose(file);
    close(sockfd);
    time_t temp_final = time(NULL) - temp_ini;
    printf("\n\nTempo total = %ld", temp_final);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    client("arquivo_de_teste.bin", "127.0.0.1");

    // printf("Uso: %s <arquivo>\n", argv[0]);
    // exit(EXIT_FAILURE);

    return 0;
}