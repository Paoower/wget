#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_array_append) {
	char **s;

	s = NULL;
	s = array_append(s, NULL);
	ck_assert_ptr_eq(s, NULL);
	free_array(s);
}
END_TEST

Suite*	tools_suite() {
	Suite *s;
	TCase *tc_string;

	s = suite_create("Tools");

	tc_string = tcase_create("String");

	tcase_add_test(tc_string, test_array_append);
	suite_add_tcase(s, tc_string);

	return s;
}
