#include "download_file_from_url.h"
#include <check.h>

START_TEST(test_remove_chunk_format)
{
	char	prev_buf[] = "aaaaaaaaaaaaaa\r\n";
	int		prev_buf_len = strlen(prev_buf);
	char	cur_buf[] = "ccccccccccccccccccc";
	int		cur_buf_len = strlen(cur_buf);
	bool	is_first_buff = false;

	remove_chunk_format(prev_buf, &prev_buf_len,
						cur_buf, &cur_buf_len, &is_first_buff);
	ck_assert_int_eq(prev_buf_len, 16);
	ck_assert_mem_eq(prev_buf, "aaaaaaaaaaaaaa\r\n", prev_buf_len);
	ck_assert_int_eq(cur_buf_len, 19);
	ck_assert_mem_eq(cur_buf, "ccccccccccccccccccc", cur_buf_len);
}
END_TEST

Suite*	download_file_suite()
{
	Suite	*s;
	TCase	*tc_write_chunked;

	s = suite_create("downdload_file");

	tc_write_chunked = tcase_create("write_chunked");
	// create tcases
	tcase_add_test(tc_write_chunked, test_remove_chunk_format);
	// add tests to tcase

	suite_add_tcase(s, tc_write_chunked);
	// add tacases to suite
	return s;
}
