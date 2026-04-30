#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "ecc.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cout << "Usage: ./client <alice|bob>\n";
        return 0;
    }

    string role = argv[1];

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&server, sizeof(server));

    cout << role << " connected\n";

    int priv;
    cout << "Enter private key: ";
    cin >> priv;

    Point G = {3,6,false};

    Point pub = multiply(G, priv);
    int pubArr[2] = {pub.x, pub.y};

    send(sock, pubArr, sizeof(pubArr), 0);

    int other[2];
    recv(sock, other, sizeof(other), 0);

    Point otherPub = {other[0], other[1], false};

    Point shared = multiply(otherPub, priv);
    int key = shared.x;

    cin.ignore();
    string msg;

    if(role == "alice")
    {
        cout << "Alice send message: ";
        getline(cin, msg);

        for(char &c : msg) c ^= key;

        send(sock, msg.c_str(), msg.size(), 0);
    }
    else
    {
        char buffer[1024];
        recv(sock, buffer, sizeof(buffer), 0);

        string received(buffer);

        for(char &c : received) c ^= key;

        cout << "Bob received: " << received << endl;
    }

    close(sock);
    return 0;
}
