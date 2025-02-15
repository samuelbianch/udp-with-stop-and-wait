# udp-with-stop-and-wait

UDP com a transferência confável de dados

## Como usar

Para usar este experimento siga os seguintes passos:

1. Inicie o servidor

```
# Compile o codigo do servidor
gcc -o servidor stop_and_wait_udp.c

# Inicie o servidor
./servidor

# Compile o cliente
gcc -o cliente stop_and_wait_client.c

# inicie o cliente
./cliente
```

Com isso, o cliente automaticamente irá enviar os dados para o servidor e simular algumas perdas.
