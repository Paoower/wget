#include "args.h"
#include <bits/getopt_core.h>

struct Parameters params = {
	.background = 0,
	.outputfile = "",
	.filepath = "",
	.ratelimit = 0,
	.linksfile = "",
	.mirror = 0
};

int handle_args(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("wget: URL manquante\n");
		return 0;
	}

	int c;
	while ((c = getopt(argc, argv, "abc:")) != 1) {
		switch (c) {
			case 'B':
				params.background = 1;
			case 'O':
				params.outputfile = optarg;
		}
	}

	

	return 0;
}