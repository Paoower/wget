#include "src.h"
#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_parse_links_from_html)
{
	int					i;
	arraystr			urls;
	struct file_data	file_data;
	struct host_data	host_data;
	char				*hostname = "wims.univ-lehavre";
	char				*file_path = "./tests/src_test_files/univ-lehavre.html";
	char				*expected_urls[] = {
							"https://wims.univ-lehavre/icons/openlogo-75.png",
							"https://wims.univ-lehavre/manual",
							NULL
	};

	file_data.file_path = file_path;
	file_data.host_data = &host_data;
	file_data.host_data->hostname = hostname;
	urls = parse_links_from_html(&file_data, NULL, NULL, true, false);
	if (!urls)
		ck_abort_msg("no urls found");
	// i = 0;
	// printf("URLS=[");
	// while (urls[i]) {
	// 	printf("\"%s\"", urls[i]);
	// 	if (urls[i + 1])
	// 		printf(", ");
	// 	i++;
	// }
	// printf("]\n");
	i = 0;
	while (expected_urls[i]) {
		if (!urls[i])
			ck_abort();
		ck_assert_str_eq(urls[i], expected_urls[i]);
		i++;
	}
	clean_arraystr(&urls);
}
END_TEST

START_TEST(test_convert_link_to_online)
{
	char				*result;
	char				*temp;
	struct file_data	file_data;
	struct host_data	host_data;
	char				*hostname = "kathdedon.files.wordpress.com";
	char				*basic_tests[] = {
								"/2010/12/baked-pie.jpg",
								"2010/12/baked-pie.jpg",
								"./2010/12/baked-pie.jpg",
								"http://kathdedon.files.wordpress.com/2010/12/baked-pie.jpg",
								"kathdedon.files.wordpress.com/2010/12/baked-pie.jpg",
								NULL
	};

	file_data.file_path = NULL;
	file_data.header_data = NULL;
	file_data.host_data = &host_data;

	host_data.hostname = hostname;
	host_data.is_secured = false;
	for (int i = 0; basic_tests[i]; i++) {
		temp = strdup(basic_tests[i]);
		result = convert_link(temp, &file_data, false);
		ck_assert_str_eq(result,
				"http://kathdedon.files.wordpress.com/2010/12/baked-pie.jpg");
		free(temp);
		free(result);
	}
}
END_TEST

START_TEST(test_convert_link_to_offline)
{
	char				*result;
	char				*temp;
	struct file_data	file_data;
	struct host_data	host_data;
	char				*hostname = "kathdedon.files.wordpress.com";
	char				*basic_tests[] = {
								"/2010/12/baked-pie.jpg",
								"2010/12/baked-pie.jpg",
								"./2010/12/baked-pie.jpg",
								"http://kathdedon.files.wordpress.com/2010/12/baked-pie.jpg",
								"kathdedon.files.wordpress.com/2010/12/baked-pie.jpg",
								NULL
	};

	file_data.file_path = NULL;
	file_data.header_data = NULL;
	file_data.host_data = &host_data;

	host_data.hostname = hostname;
	for (int i = 0; basic_tests[i]; i++) {
		temp = strdup(basic_tests[i]);
		result = convert_link(temp, &file_data, true);
		ck_assert_str_eq(result, "2010/12/baked-pie.jpg");
		free(temp);
		free(result);
	}
}
END_TEST

Suite*	src_suite()
{
	Suite	*s;
	TCase	*tc_mirror;
	TCase	*tc_convert_links;

	s = suite_create("Src");

	tc_convert_links = tcase_create("Convert Links");
	tc_mirror = tcase_create("Mirror");
	// create tcases

	tcase_add_test(tc_convert_links, test_convert_link_to_online);
	tcase_add_test(tc_convert_links, test_convert_link_to_offline);
	tcase_add_test(tc_mirror, test_parse_links_from_html);
	// add tests to tcase

	suite_add_tcase(s, tc_convert_links);
	suite_add_tcase(s, tc_mirror);
	// add tacases to suite
	return s;
}
