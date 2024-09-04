#include "args.h"
#include "tools.h"

struct parameters_t params = {
	.background = 0,
	.output_file = NULL,
	.file_path = "./data/",
	.rate_limit = 0,
	.links_file = NULL,
	.mirror = 0,
	.reject_list = NULL,
	.exclude_list = NULL,
	.convert_links = 0,
};

struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"background", no_argument, 0, 'B'},
	{"outputfile", required_argument, 0, 'O'},
	{"filepath", required_argument, 0, 'P'},
	{"rate-limit", required_argument, 0, 'l'},
	{"linksfile", required_argument, 0, 'i'},
	{"mirror", no_argument, 0, 'm'},
	{"reject", required_argument, 0, 'R'},
	{"exclude", required_argument, 0, 'X'},
	{"convert-links", optional_argument, 0, 'C'},
	{0, 0, 0, 0},
};

/*
Displays an option.

Parameters:
- option: the current option
- value: the option's value
*/
void display_arg(char option, char *value)
{
	if (value)
	{
		printf("Used option -%c with value %s\n", option, value);
	}
	else
	{
		printf("Used option -%c\n", option);
	}
}

int handle_args(int argc, char *argv[])
{
	// if (argc == 1)
	// {
	// 	printf("wget: Missing URL\n");
	// 	return 0;
	// }

	char *optstring = "hBO:P:l:i:mR:X:C";
	int option_index = 0;

	int opt;
	while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)
	{
		switch (opt)
		{
		case 'h':
			printf("Usage: wget [-h] [-B] [-O <outputfile>] [-P <filepath>] [--rate-limit <speed>] [-i <filename>] [--mirror] [--reject | -R <list>] [--exclude | -X <list>] [--convert-links links] url\n");
			printf("\n\tOptions");
			printf("\n\t -h,\t Help flag, displays this.\n");
			printf("\n\t -B,\t Downloads the file immediately in the background.\n");
			printf("\n\t -O <filename>,\t Save a file under a different name.\n");
			printf("\n\t -P <pathname>,\t Specify the path where the file is going to be saved.\n");
			printf("\n\t -i <filename>,\t Downloads from all the links that are in the provided file \n");
			printf("\n\t --rate-limit,\t Limits the speed of your download.\n");
			printf("\n\t --mirror,\t Downloads an entire website.\n");
			printf("\n\nAll the following flags only work with --mirror :\n");
			printf("\n\t --reject <list> | -R <list>,\t Downloads all files except the ones matching the suffixes in the list.\n");
			printf("\n\t --exclude <list> | -X <list>,\t List of directories excluded from the download.\n");
			printf("\n\t --convert-links,\t Converts the links in the downloaded files so that they can be viewed offline.\n");
			break;
		case 'B':
			params.background = 1;
			display_arg(opt, NULL);
			break;
		case 'O':
			if (optarg)
			{
				params.output_file = optarg;
			}
			display_arg(opt, optarg);
			break;
		case 'P':
			if (optarg)
			{
				params.file_path = optarg;
			}
			display_arg(opt, optarg);
			break;
		case 'i':
			if (optarg)
			{
				params.links_file = optarg;
			}
			display_arg(opt, optarg);
			break;
		case 'm':
			if (optarg)
			{
				params.mirror = 1;
			}
			display_arg(opt, optarg);
			break;
		case 'R':
			if (optarg && params.mirror)
			{
				params.reject_list = optarg;
			}
			display_arg(opt, optarg);
			break;
		case 'X':
			if (optarg && params.mirror)
			{
				params.exclude_list = optarg;
			}
			display_arg(opt, optarg);
			break;
		case 'C':
			if (params.mirror)
			{
				params.convert_links = 1;
			}
			display_arg(opt, NULL);
			break;
		case ':':
			fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			exit(EXIT_FAILURE);
		case '?':
			fprintf(stderr, "Unknown option: -%c\n", optopt);
			exit(EXIT_FAILURE);
		default:
			fprintf(stderr, "Usage: wget [-h] [-B] [-O <outputfile>] [-P <filepath>] [--rate-limit <speed>] [-i <filename>] [--mirror] [--reject | -R <list>] [--exclude | -X <list>] [--convert-links links] url\n");
			exit(EXIT_FAILURE);
		}
	}

	if (does_match_with_pattern(argv[optind], URL_REGEX))
	{
		params.url = argv[optind];
	}
	else
	{
		fprintf(stderr, "wget: URL missing.\n");
		exit(EXIT_FAILURE);
	}

	parse_url();

	return 0;
}