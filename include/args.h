#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

struct parameters_t
{
	char *url;
	char *url_filename;
	char *url_hostname;
	char *url_filepath;
	int background;
	char *output_file;
	char *file_path;
	int rate_limit;
	char *links_file;
	int mirror;
	char *reject_list;
	char *exclude_list;
	int convert_links;
};

extern struct parameters_t params;

#endif