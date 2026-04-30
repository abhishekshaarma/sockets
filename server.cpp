#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "ecc.h"

using namespace std;

void showEncrypt(string msg, int key)
{
    cout << "\n--- Encryption Process ---\n";
    cout << "Plaintext: " << msg << "\n";
    cout << "Key (shared secret x): " << key << "\n";

    cout << "Ciphertext (ASCII values): ";

    for(char c : msg)
    {
        char enc = c ^ key;
        cout << (int)enc << " ";
    }

    cout << "\n--------------------------\n";
}

int main(int argc, char* argv[])
{
    string role = (argc > 1) ? argv[1] : "client";

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&server, sizeof(server));

    cout << "\n[" << role << "] Connected to server\n";

    // STEP 1: private key
    int priv;
    cout << "Enter private key: ";
    cin >> priv;

    cout << "\n--- ECC KEY GENERATION ---\n";
    cout << "Private key = " << priv << "\n";

    Point G = {3, 6, false};
    cout << "Base point G = (" << G.x << "," << G.y << ")\n";

    // STEP 2: public key
    Point pub = multiply(G, priv);
    cout << "Public key = kG = (" << pub.x << "," << pub.y << ")\n";

    int pubArr[2] = {pub.x, pub.y};
    send(sock, pubArr, sizeof(pubArr), 0);

    // STEP 3: receive other public key
    int other[2];
    recv(sock, other, sizeof(other), 0);

    Point otherPub = {other[0], other[1], false};
    cout << "Received public key = (" << otherPub.x << "," << otherPub.y << ")\n";

    // STEP 4: shared secret
    Point shared = multiply(otherPub, priv);
    cout << "Shared secret point = (" << shared.x << "," << shared.y << ")\n";

    int key = shared.x;
    cout << "Derived symmetric key = " << key << "\n";

    cin.ignore();
    string msg;

    if(role == "alice")
    {
        cout << "\nEnter message: ";
        getline(cin, msg);

        showEncrypt(msg, key);

        for(char &c : msg)
            c ^= key;

        send(sock, msg.c_str(), msg.size(), 0);

        cout << "Encrypted message sent\n";
    }
    else
    {
        char buffer[1024];
        recv(sock, buffer, sizeof(buffer), 0);

        string received(buffer);

        cout << "\nEncrypted received message (raw): " << received << "\n";

        for(char &c : received)
            c ^= key;

        cout << "Decrypted message: " << received << "\n";
    }

    close(sock);
    return 0;
}
