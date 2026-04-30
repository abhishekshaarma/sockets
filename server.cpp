#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main()
{
    int server_fd, client1, client2;
    sockaddr_in address;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 2);

    cout << "Server started on port 8080...\n";
    cout << "Waiting for Alice...\n";

    client1 = accept(server_fd, nullptr, nullptr);
    cout << "Alice connected.\n";

    cout << "Waiting for Bob...\n";
    client2 = accept(server_fd, nullptr, nullptr);
    cout << "Bob connected.\n";

    // receive Alice public key
    int aliceKey[2];
    recv(client1, aliceKey, sizeof(aliceKey), 0);

    // receive Bob public key
    int bobKey[2];
    recv(client2, bobKey, sizeof(bobKey), 0);

    // send Bob key to Alice
    send(client1, bobKey, sizeof(bobKey), 0);

    // send Alice key to Bob
    send(client2, aliceKey, sizeof(aliceKey), 0);

    cout << "\nPublic keys exchanged.\n";

    char buffer[1024];

    // receive encrypted msg from Alice
    recv(client1, buffer, sizeof(buffer), 0);

    // forward to Bob
    send(client2, buffer, sizeof(buffer), 0);

    cout << "Encrypted message forwarded.\n";

    close(client1);
    close(client2);
    close(server_fd);

    return 0;
}
