//function of the system
#ifndef SYS_H
#define SYS_H

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace sys {
	int ls(string, vector<string>&);//return a list of the files in the directory of the argument #1
}
#endif

