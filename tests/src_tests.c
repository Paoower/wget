#include "wget_mirror.h"
#include <check.h>
#include <stdio.h>

START_TEST(test_get_urls_from_html) {
	int		i;
	char	**urls;
	char	*expected_urls[] = {"/icons/openlogo-75.png",
								"/manual",
								"http://httpd.apache.org/docs/2.4/mod/mod_userdir.html",
								"http://bugs.debian.org/cgi-bin/pkgreport.cgi?ordering=normal;archive=0;src=apache2;repeatmerged=0"
								};
	urls = get_urls_from_html("./tests/src_test_files/univ-lehavre.html", NULL, NULL);
	if (!urls)
		ck_abort();
	i = 0;
	printf("URLS=[");
	while (urls[i]) {
		printf("\"%s\"", urls[i]);
		if (urls[i + 1])
			printf(", ");
		i++;
	}
	printf("]\n");
	i = 0;
	while (expected_urls[i]) {
		if (!urls[i])
			ck_abort();
		ck_assert_str_eq(urls[i], expected_urls[i]);
		i++;
	}
}
END_TEST

Suite*	src_suite() {
	Suite *s;
	TCase *tc_mirror;

	s = suite_create("Src");

	tc_mirror = tcase_create("Mirror");
	// create tcases

	tcase_add_test(tc_mirror, test_get_urls_from_html);
	// add tests to tcase

	suite_add_tcase(s, tc_mirror);
	// add tacases to suite
	return s;
}
