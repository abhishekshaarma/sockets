#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 2);

    cout << "[SERVER] Waiting...\n";

    int A = accept(server_fd, NULL, NULL);
    cout << "[SERVER] Alice connected\n";

    int B = accept(server_fd, NULL, NULL);
    cout << "[SERVER] Bob connected\n";

    int alicePub[2], bobPub[2];

    recv(A, alicePub, sizeof(alicePub), 0);
    recv(B, bobPub, sizeof(bobPub), 0);

    send(A, bobPub, sizeof(bobPub), 0);
    send(B, alicePub, sizeof(alicePub), 0);

    cout << "[SERVER] ECC keys exchanged\n";

    char buffer[1024];

    while(true)
    {
        // Alice -> Bob
        int n = recv(A, buffer, sizeof(buffer), 0);
        if(n <= 0) break;

        send(B, buffer, n, 0);

        // Bob -> Alice
        n = recv(B, buffer, sizeof(buffer), 0);
        if(n <= 0) break;

        send(A, buffer, n, 0);
    }

    close(A);
    close(B);
    close(server_fd);

    return 0;
}
