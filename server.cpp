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

    cout << "[SERVER] Waiting for clients...\n";

    int A = accept(server_fd, NULL, NULL);
    cout << "[SERVER] Alice connected\n";

    int B = accept(server_fd, NULL, NULL);
    cout << "[SERVER] Bob connected\n";

    int alicePub[2], bobPub[2];

    cout << "[SERVER] Receiving public keys...\n";

    recv(A, alicePub, sizeof(alicePub), 0);
    recv(B, bobPub, sizeof(bobPub), 0);

    cout << "[SERVER] Exchanging keys...\n";

    send(A, bobPub, sizeof(bobPub), 0);
    send(B, alicePub, sizeof(alicePub), 0);

    cout << "[SERVER] Keys exchanged successfully\n";

    char buffer[1024];

    recv(A, buffer, sizeof(buffer), 0);
    cout << "[SERVER] Message received from Alice\n";

    send(B, buffer, sizeof(buffer), 0);
    cout << "[SERVER] Message forwarded to Bob\n";

    close(A);
    close(B);
    close(server_fd);

    return 0;
}
