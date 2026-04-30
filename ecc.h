#ifndef ECC_H
#define ECC_H

#include <iostream>
using namespace std;

const int A = 2;
const int P = 97;

struct Point
{
    int x;
    int y;
    bool inf;
};

Point INF = {0,0,true};

int mod(int x)
{
    x %= P;
    if(x < 0) x += P;
    return x;
}

int inverse(int n)
{
    int t = 0, newt = 1;
    int r = P, newr = mod(n);

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

    if(t < 0) t += P;
    return t;
}

Point add(Point P1, Point P2)
{
    if(P1.inf) return P2;
    if(P2.inf) return P1;

    if(P1.x == P2.x && mod(P1.y + P2.y) == 0)
        return INF;

    int lambda;

    if(P1.x == P2.x && P1.y == P2.y)
    {
        int num = mod(3 * P1.x * P1.x + A);
        int den = inverse(2 * P1.y);
        lambda = mod(num * den);
    }
    else
    {
        int num = mod(P2.y - P1.y);
        int den = inverse(P2.x - P1.x);
        lambda = mod(num * den);
    }

    int xr = mod(lambda*lambda - P1.x - P2.x);
    int yr = mod(lambda*(P1.x - xr) - P1.y);

    return {xr, yr, false};
}

Point multiply(Point G, int k)
{
    Point result = INF;
    Point cur = G;

    while(k > 0)
    {
        if(k % 2 == 1)
            result = add(result, cur);

        cur = add(cur, cur);
        k /= 2;
    }

    return result;
}

#endif
