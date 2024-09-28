#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_array_append) {
	char	**array;

	array = NULL;
	array_append(&array, NULL);
	ck_assert_ptr_eq(array, NULL);
	array_append(&array, "coucou");
	ck_assert_ptr_ne(array, NULL);
	if (array) {
		ck_assert_str_eq(array[0], "coucou");
		ck_assert_ptr_eq(array[1], NULL);
		array_append(&array, "liamine");
		ck_assert_str_eq(array[1], "liamine");
		ck_assert_ptr_eq(array[2], NULL);
	} else {
		ck_abort();
		free_array(array);
	}
	free_array(array);
}
END_TEST

START_TEST(test_array_concat) {
	char	**array1;
	char	**array2;
	char	**result;

	array1 = NULL;
	array2 = NULL;
	result = NULL;
	array_concat(&result, NULL);
	ck_assert_ptr_eq(result, NULL);
	array_append(&array1, "a1");
	array_concat(&result, array1);
	if (!result)
		goto error_escape;
	ck_assert_str_eq(result[0], "a1");
	array_append(&array2, "b1");
	array_append(&array2, "b2");
	array_concat(&result, array2);
	ck_assert_str_eq(result[0], "a1");
	ck_assert_str_eq(result[2], "b2");
	ck_assert_ptr_eq(result[3], NULL);
	free_array(array1);
	free_array(array2);
	free_array(result);
	return;
error_escape:
	free_array(array1);
	free_array(array2);
	free_array(result);
	ck_abort();
}
END_TEST

Suite*	tools_suite() {
	Suite *s;
	TCase *tc_string;

	s = suite_create("Tools");

	tc_string = tcase_create("String");
	// create tcases

	tcase_add_test(tc_string, test_array_append);
	tcase_add_test(tc_string, test_array_concat);
	// add tests to tcase

	suite_add_tcase(s, tc_string);
	// add tacases to suite
	return s;
}
