#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define PORT 8080
#define PACKET_SIZE 1024
#define LOSS_PROBABILITY 10 // 10% chance de perda

// Estrutura do pacote
typedef struct {
    int seq_num;
    int data_size;
    char data[PACKET_SIZE];
} Packet;

int packet_lost() {
    return (rand() % 100) < LOSS_PROBABILITY;
}

// Servidor UDP
void server() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    Packet packet;
    int expected_seq = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao associar socket");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen("output.bin", "wb");
    if (!file) {
        perror("Erro ao criar arquivo");
        exit(EXIT_FAILURE);
    }

    while (1) {
        recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, &addr_len);

        if (packet.data_size == 0) {
            printf("Recebimento concluído.\n");
            break;
        }
        
        if (!packet_lost()) {
            if (packet.seq_num == expected_seq) {
                fwrite(packet.data, 1, PACKET_SIZE, file);
            }
            sendto(sockfd, &expected_seq, sizeof(expected_seq), 0, (struct sockaddr *)&client_addr, addr_len);
            expected_seq++;
        } else {
            printf("Pacote %d perdido (simulado)\n", packet.seq_num);
        }
    }

    fclose(file);
    close(sockfd);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    server();
    return 0;
}
