#include"../include/sys.h"


int sys::ls(string dir, vector<string> &filesList){
	DIR *dp;
	struct dirent *dirp;

	if((dp = opendir(dir.c_str())) == NULL){
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp= readdir(dp)) != NULL){

		if ( string(dirp->d_name) == "." || string(dirp->d_name) == ".." ){
			continue;
		}
		else{

			filesList.push_back(string(dirp->d_name));
		}
	}

	closedir(dp);

	return 0;

};

