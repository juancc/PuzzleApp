#ifndef OBJ_H
#define OBJ_H

#include<iostream>

using namespace std;

class Obj
{
    public:
        Obj();
        Obj(int nx, int ny, int nidx, string nName, double ndst, double* hu);
        virtual ~Obj();

        int x;//position of the mass center of the region
        int y;

        int idx;
        string objName;
        double dst;//distance to the candidate object
        double* hu;

    private:
};

#endif // OBJ_H
