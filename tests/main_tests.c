#include "tests.h"
#include <stdio.h>

int	main()
{
	int number_failed;
	SRunner *sr;

	sr = srunner_create(tools_suite());
	srunner_add_suite(sr, src_suite());
	// srunner_add_suite(sr, MY_SUITE)

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? 0 : 1;
}
