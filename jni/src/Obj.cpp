#include "../include/Obj.h"

/*
storage parameters of an identified region
*/
Obj::Obj(int nx, int ny, int nidx, string nname, double ndst, double* nHu) : x(nx), y(ny), idx(nidx), objName(nname), dst(ndst)
{
    //copying the hu array
    hu = new double[7];
    for(unsigned int i=0; i<7; i++){
        hu[i] = nHu[i];
    }
}


Obj::~Obj()
{
    //dtor
}
