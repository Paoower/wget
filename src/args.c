#include "src.h"
#include "tools.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>

void free_args(struct parameters_t *params)
{
	if (!params)
		return;
	free(params->output_file);
	free(params->storage_path);
	free(params->links_file);
	free(params->reject_list);
	free(params->exclude_list);
	free(params->url);
}

/**
 * Converts a rate limit expressed as a string into a number
 * @param rate_limit Rate limit as a string
 * @return Integer value of the rate limit
 */
// static unsigned long get_bytes_per_sec(char *rate_limit)
// {
// 	// Search for M or K character
// 	char *found = strchr(rate_limit, 'k');
// 	char *newstr;
// 	char *check_ptr;
// 	unsigned long rate;

// 	if (found)
// 	{
// 		newstr = malloc((found - rate_limit) + 1);
// 		strncpy(newstr, rate_limit, found - rate_limit);
// 		newstr[found - rate_limit] = '\0';
// 		rate = strtoul(newstr, &check_ptr, 10);
// 		if (check_ptr == newstr || rate <= 0 || rate * 1000 > ULONG_MAX)
// 		{
// 			free(newstr);
// 			return 0;
// 		}
// 		free(newstr);
// 		rate *= 1000; // convert kilobyte to byte
// 	}
// 	return rate;
// }

/**
 * @brief Parses the option whether it contains "="
 * @param optarg Option to parse
 * @return Parsed option
*/
static char* parse_equal(char* optarg) {
	char *s = strstr(optarg, "=");

	if (s)
		return s + 1;

	return optarg;
}

/**
 * Sort options and arguments into the parameters struct
 * @param parameters Struct where all option are going to be stored
 * @param argc Number of arguments
 * @param argv List of arguments
 *
 * @return A boolean
 */
int handle_args(struct parameters_t *parameters, int argc, char *argv[])
{
	// Option string
	char *optstring = "hBO:P:l:i:mR:X:C";
	int option_index = 0;
	int opt;

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

	if (argc < 2)
	{
		fprintf(stderr, "wget: Not enough args.\n");
		exit(EXIT_FAILURE);
	}

	// Loop that goes through each option and checks different cases
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
			parameters->background = 1;
			break;

		case 'O':
			copy_string(&parameters->output_file, optarg ? parse_equal(optarg) : strdup("test.mp4"));
			break;

		case 'P':
			copy_string(&parameters->storage_path, optarg ? parse_equal(optarg) : strdup("./data/"));
			break;

		case 'l':
			if (optarg)
				parameters->rate_limit = metric_to_basic_decimal(
														parse_equal(optarg));
			break;

		case 'i':
			copy_string(&parameters->links_file, optarg ? parse_equal(optarg) : strdup(""));
			break;

		case 'm':
			parameters->mirror = 1;
			break;

		case 'R':
			if (optarg && parameters->mirror)
			{
				copy_string(&parameters->reject_list, parse_equal(optarg));
			}
			break;

		case 'X':
			if (optarg && parameters->mirror)
			{
				copy_string(&parameters->exclude_list, parse_equal(optarg));
			}
			break;

		case 'C':
			parameters->convert_links = 1;
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

	// Check if there is an URL
	if(!parameters->links_file)
	{
		if (optind >= argc && argv[optind] == NULL)
		{
			fprintf(stderr, "wget: URL missing.\n");
			exit(EXIT_FAILURE);
		}

		// Check if the URL is valid
		if (does_match_with_pattern(argv[optind], URL_REGEX))
		{
			copy_string(&parameters->url, argv[optind]);
		}
		else
		{
			fprintf(stderr, "wget: Invalid URL.\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
