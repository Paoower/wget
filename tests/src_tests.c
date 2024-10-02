#include "wget_mirror.h"
#include "src.h"
#include "tools.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_get_urls_from_html)
{
	int					i;
	arraystr			urls;
	struct file_data	file_data;
	char				*file_path = "./tests/src_test_files/univ-lehavre.html";
	char				*expected_urls[] = {
							"/icons/openlogo-75.png",
							"/manual",
							"http://httpd.apache.org/docs/2.4/mod/mod_userdir.html",
							"http://bugs.debian.org/cgi-bin/pkgreport.cgi?ordering=normal;archive=0;src=apache2;repeatmerged=0"
	};

	file_data.file_path = file_path;
	urls = parse_links_from_html(&file_data, NULL, NULL);
	if (!urls)
		ck_abort();
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
		result = convert_link(basic_tests[i], &file_data, false);
		ck_assert_str_eq(result,
				"http://kathdedon.files.wordpress.com/2010/12/baked-pie.jpg");
		free(result);
	}
}
END_TEST

START_TEST(test_convert_link_to_offline)
{
	char				*result;
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
		result = convert_link(basic_tests[i], &file_data, true);
		ck_assert_str_eq(result, "2010/12/baked-pie.jpg");
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
	tcase_add_test(tc_mirror, test_get_urls_from_html);
	// add tests to tcase

	suite_add_tcase(s, tc_convert_links);
	suite_add_tcase(s, tc_mirror);
	// add tacases to suite
	return s;
}
