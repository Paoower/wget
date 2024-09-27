#include "tests.h"
#include <stdio.h>

int	main()
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = tools_suite();
	sr = srunner_create(s);
	// srunner_add_suite(sr, MY_SUITE)

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? 0 : 1;
}
