#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

struct Parameters
{
	int background;
	char *outputfile;
	char *filepath;
	int ratelimit;
	char *linksfile;
	int mirror;
};

extern struct Parameters params;


#endif