
#include <iostream>
using namespace std;

const int a = 2;
const int b = 3;
const int p = 97;

struct Point
{
    int x;
    int y;
    bool infinity;
};

Point INF = {0,0,true};

int mod(int x)
{
    x %= p;
    if(x < 0) x += p;
    return x;
}

int modInverse(int n)
{
    int t = 0, newt = 1;
    int r = p, newr = mod(n);

    while(newr != 0)
    {
        int q = r / newr;

        int temp = newt;
        newt = t - q * newt;
        t = temp;

        temp = newr;
        newr = r - q * newr;
        r = temp;
    }

    if(t < 0) t += p;
    return t;
}

void printPoint(Point P)
{
    if(P.infinity) cout << "INF";
    else cout << "(" << P.x << "," << P.y << ")";
}

Point add(Point P, Point Q)
{
    if(P.infinity) return Q;
    if(Q.infinity) return P;

    if(P.x == Q.x && mod(P.y + Q.y) == 0)
        return INF;

    int lambda;

    if(P.x == Q.x && P.y == Q.y)
    {
        int num = mod(3 * P.x * P.x + a);
        int den = modInverse(2 * P.y);
        lambda = mod(num * den);
    }
    else
    {
        int num = mod(Q.y - P.y);
        int den = modInverse(Q.x - P.x);
        lambda = mod(num * den);
    }

    int xr = mod(lambda * lambda - P.x - Q.x);
    int yr = mod(lambda * (P.x - xr) - P.y);

    return {xr, yr, false};
}

Point scalarMultiply(Point P, int k)
{
    Point result = INF;
    Point current = P;

    while(k > 0)
    {
        if(k % 2 == 1)
            result = add(result, current);

        current = add(current, current);
        k /= 2;
    }

    return result;
}

int main()
{
    Point G = {3,6,false};

    int alicePrivate = 7;
    int bobPrivate   = 11;

    Point alicePublic = scalarMultiply(G, alicePrivate);
    Point bobPublic   = scalarMultiply(G, bobPrivate);

    Point aliceShared = scalarMultiply(bobPublic, alicePrivate);
    Point bobShared   = scalarMultiply(alicePublic, bobPrivate);

    cout << "Base Point G = ";
    printPoint(G);

    cout << "\n\nAlice Private Key = " << alicePrivate;
    cout << "\nAlice Public Key = ";
    printPoint(alicePublic);

    cout << "\n\nBob Private Key = " << bobPrivate;
    cout << "\nBob Public Key = ";
    printPoint(bobPublic);

    cout << "\n\nAlice Shared Secret = ";
    printPoint(aliceShared);

    cout << "\nBob Shared Secret = ";
    printPoint(bobShared);

    cout << endl;
}
a
