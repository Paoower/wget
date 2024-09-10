#include <stdio.h>
#include <regex.h>

int	does_match_with_pattern(const char *str, char *pattern)
{
	regex_t	regex;
	int		ret;

	ret = regcomp(&regex, pattern, REG_EXTENDED);
	if (ret) {
		fprintf(stderr, "Could not compile regex\n");
		return 0;
	}
	ret = regexec(&regex, str, 0, NULL, 0);
	regfree(&regex);
	return !ret;
}
