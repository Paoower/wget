#include "tests.h"
#include <stdio.h>

int	main()
{
	int		i;
	SRunner	*sr;
	Suite	*suites[] = {
		src_suite(),
		tools_suite(),
		NULL
	};

	i = 0;
	while (suites[i]) {
		sr = srunner_create(suites[i]);
		srunner_run(sr, NULL, NULL, CK_NORMAL);
		if (suites[i + 1])
			printf("\n");
		srunner_free(sr);
		i++;
	}
}
