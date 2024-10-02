#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_array_init)
{
	arraystr	array;

	array = arraystr_init(NULL);
	ck_assert_ptr_eq(array, NULL);
	array = arraystr_init("coucou", "yo", NULL);
	if (!array)
		ck_abort();
	ck_assert_str_eq(array[0], "coucou");
	ck_assert_str_eq(array[1], "yo");
	ck_assert_ptr_eq(array[2], NULL);
	clean_arraystr(&array);
}
END_TEST

START_TEST(test_array_append)
{
	arraystr	array;

	array = arraystr_init(NULL);
	ck_assert_ptr_eq(array, NULL);
	arraystr_append(&array, "coucou");
	ck_assert_ptr_ne(array, NULL);
	if (array) {
		ck_assert_str_eq(array[0], "coucou");
		ck_assert_ptr_eq(array[1], NULL);
		arraystr_append(&array, "liamine");
		ck_assert_str_eq(array[1], "liamine");
		ck_assert_ptr_eq(array[2], NULL);
	} else {
		ck_abort();
		clean_arraystr(&array);
	}
	clean_arraystr(&array);
}
END_TEST

START_TEST(test_array_merge)
{
	arraystr	array1;
	arraystr	array2;
	arraystr	result;

	result = arraystr_init(NULL);
	ck_assert_ptr_eq(result, NULL);
	array1 = arraystr_init("a1", NULL);
	arraystr_merge(&result, array1);
	if (!result)
		goto error_escape;
	ck_assert_str_eq(result[0], "a1");
	array2 = arraystr_init("b1", "b2", NULL);
	arraystr_merge(&result, array2);
	ck_assert_str_eq(result[0], "a1");
	ck_assert_str_eq(result[2], "b2");
	ck_assert_ptr_eq(result[3], NULL);
	clean_arraystr(&array1);
	clean_arraystr(&array2);
	clean_arraystr(&result);
	return;
error_escape:
	clean_arraystr(&array1);
	clean_arraystr(&array2);
	clean_arraystr(&result);
	ck_abort();
}
END_TEST

START_TEST(test_array_deduplicate)
{
	arraystr	arr;

	arr = arraystr_init("1", "2", "2a", "2", "1", "1a", "3a", "3", "3", NULL);
	arraystr_deduplicate(&arr);
	if (arraystr_len(arr) < 6)
		ck_abort();
	ck_assert_str_eq(arr[0], "1");
	ck_assert_str_eq(arr[1], "2");
	ck_assert_str_eq(arr[2], "2a");
	ck_assert_str_eq(arr[3], "1a");
	ck_assert_str_eq(arr[4], "3a");
	ck_assert_str_eq(arr[5], "3");
	ck_assert_ptr_eq(arr[6], NULL);
	clean_arraystr(&arr);
}
END_TEST

START_TEST(test_array_join)
{
	char	*joined;
	arraystr	arr;

	arr = arraystr_init("a", "bc", "d", "efg", NULL);
	joined = arraystr_join(arr);
	if (!joined)
		ck_abort();
	ck_assert_str_eq(joined, "abcdefg");
	free_arraystr(arr);
	free(joined);
}
END_TEST

Suite*	tools_suite()
{
	Suite *s;
	TCase *tc_array;

	s = suite_create("Tools");

	tc_array = tcase_create("Array");
	// create tcases

	tcase_add_test(tc_array, test_array_init);
	tcase_add_test(tc_array, test_array_append);
	tcase_add_test(tc_array, test_array_merge);
	tcase_add_test(tc_array, test_array_deduplicate);
	tcase_add_test(tc_array, test_array_join);
	// add tests to tcase

	suite_add_tcase(s, tc_array);
	// add tacases to suite
	return s;
}
