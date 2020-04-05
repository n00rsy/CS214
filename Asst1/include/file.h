#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "include/arraylist.h"

#ifndef FILE_H
#define FILE_H

int compress_file(char* file_path, ArrayList *codebook);


// TODO does not fix
void write_file( char *file_path, const char *outname);

#endif
