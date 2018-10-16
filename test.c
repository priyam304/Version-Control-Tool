#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"


int main(){
	fs_cp("beargit.c","test/.beargit.c",NULL);
	return 0;
}