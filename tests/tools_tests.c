#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_array_append) {
	char **s;

	s = NULL;
	s = array_append(s, NULL);
	ck_assert_ptr_eq(s, NULL);
	s = array_append(s, "coucou");
	ck_assert_ptr_ne(s, NULL);
	if (s) {
		ck_assert_str_eq(s[0], "coucou");
		ck_assert_ptr_eq(s[1], NULL);
		s = array_append(s, "liamine");
		ck_assert_str_eq(s[1], "liamine");
		ck_assert_ptr_eq(s[2], NULL);
	}
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
