#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ecc.h"

using namespace std;

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&server, sizeof(server));

    cout << "Connected to server\n";

    int priv;
    cout << "Enter private key: ";
    cin >> priv;

    Point G = {3, 6, false};

    Point pub = multiply(G, priv);
    int pubArr[2] = {pub.x, pub.y};

    send(sock, pubArr, sizeof(pubArr), 0);

    int other[2];
    recv(sock, other, sizeof(other), 0);

    Point otherPub = {other[0], other[1], false};

    Point shared = multiply(otherPub, priv);

    cout << "Shared secret: (" << shared.x << "," << shared.y << ")\n";

    int key = shared.x;

    cin.ignore();
    string msg;

    cout << "Enter message: ";
    getline(cin, msg);

    for(char &c : msg)
        c ^= key;

    send(sock, msg.c_str(), msg.size(), 0);

    cout << "Encrypted message sent\n";

    close(sock);
    return 0;
}
