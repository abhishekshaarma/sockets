#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "ecc.h"

using namespace std;

int main(int argc, char* argv[])
{
    string role = (argc > 1) ? argv[1] : "alice";

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&server, sizeof(server));

    cout << "[" << role << "] Connected\n";

    // ---------------- ECC SETUP ----------------
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

    cout << "[ECC] Shared key = " << key << "\n";

    cin.ignore();

    char buffer[1024];

    // ---------------- CHAT LOOP ----------------
    while(true)
    {
        if(role == "alice")
        {
            string msg;
            cout << "Alice > ";
            getline(cin, msg);

            if(msg == "exit")
            {
                send(sock, "EXIT", 4, 0);
                break;
            }

            for(char &c : msg)
                c ^= key;

            string packet = "MSG:" + msg;
            send(sock, packet.c_str(), packet.size(), 0);

            cout << "[sent]\n";

            // receive reply
            recv(sock, buffer, sizeof(buffer), 0);
            string reply(buffer);

            if(reply == "EXIT") break;

            if(reply.find("MSG:") == 0)
            {
                reply = reply.substr(4);

                for(char &c : reply)
                    c ^= key;

                cout << "Bob > " << reply << endl;
            }
        }
        else // BOB
        {
            cout << "Waiting for Alice...\n";

            recv(sock, buffer, sizeof(buffer), 0);
            string msg(buffer);

            if(msg == "EXIT") break;

            if(msg.find("MSG:") == 0)
            {
                msg = msg.substr(4);

                for(char &c : msg)
                    c ^= key;

                cout << "Alice > " << msg << endl;
            }

            string reply;
            cout << "Bob > ";
            getline(cin, reply);

            if(reply == "exit")
            {
                send(sock, "EXIT", 4, 0);
                break;
            }

            for(char &c : reply)
                c ^= key;

            string packet = "MSG:" + reply;
            send(sock, packet.c_str(), packet.size(), 0);
        }
    }

    close(sock);
    cout << "[" << role << "] Disconnected\n";

    return 0;
}
