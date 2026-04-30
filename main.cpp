#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ecc.h"

using namespace std;

int main(int argc, char* argv[])
{
    string role = (argc > 1) ? argv[1] : "client";

    cout << "[CLIENT] Started as " << role << "\n";

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    cout << "[CLIENT] Connecting...\n";

    if(connect(sock, (sockaddr*)&server, sizeof(server)) < 0)
    {
        cout << "[ERROR] Connection failed\n";
        return 0;
    }

    cout << "[CLIENT] Connected to server\n";

    // ---------------- PRIVATE KEY ----------------
    int priv;
    cout << "Enter private key: ";
    cin >> priv;

    cout << "[ECC] Private key = " << priv << "\n";

    Point G = {3,6,false};

    // ---------------- PUBLIC KEY ----------------
    Point pub = multiply(G, priv);
    cout << "[ECC] Public key = (" << pub.x << "," << pub.y << ")\n";

    int pubArr[2] = {pub.x, pub.y};

    cout << "[CLIENT] Sending public key...\n";
    send(sock, pubArr, sizeof(pubArr), 0);

    // ---------------- RECEIVE KEY ----------------
    int other[2];

    cout << "[CLIENT] Waiting for other public key...\n";
    recv(sock, other, sizeof(other), 0);

    Point otherPub = {other[0], other[1], false};

    cout << "[CLIENT] Received public key = ("
         << otherPub.x << "," << otherPub.y << ")\n";

    // ---------------- SHARED SECRET ----------------
    Point shared = multiply(otherPub, priv);

    cout << "[ECC] Shared secret = (" 
         << shared.x << "," << shared.y << ")\n";

    int key = shared.x;

    cout << "[ECC] Derived key = " << key << "\n";

    cin.ignore();

    // ---------------- MESSAGE ----------------
    if(role == "alice")
    {
        string msg;
        cout << "Enter message: ";
        getline(cin, msg);

        cout << "[ENC] Original: " << msg << "\n";

        for(char &c : msg)
            c ^= key;

        cout << "[CLIENT] Sending encrypted message...\n";

        send(sock, msg.c_str(), msg.size(), 0);
    }
    else
    {
        char buffer[1024];

        cout << "[CLIENT] Waiting for message...\n";
        recv(sock, buffer, sizeof(buffer), 0);

        string msg(buffer);

        cout << "[ENC] Encrypted received: " << msg << "\n";

        for(char &c : msg)
            c ^= key;

        cout << "[DEC] Decrypted message: " << msg << "\n";
    }

    close(sock);

    cout << "[CLIENT] Done\n";
    return 0;
}
