#include "tests.h"
#include <stdio.h>

int	main()
{
	SRunner	*sr_src;
	SRunner	*sr_tools;

	sr_src = srunner_create(src_suite());
	sr_tools = srunner_create(tools_suite());

	srunner_run(sr_src, NULL, NULL, CK_NORMAL);
	srunner_run(sr_tools, NULL, NULL, CK_NORMAL);

	srunner_free(sr_src);
	srunner_free(sr_tools);
}
