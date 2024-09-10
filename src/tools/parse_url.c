#include <string.h>
#include <stdio.h>

int get_hostname(const char *url)
{
	char *s = strstr(url, "://");

	if (s)
	{
		s += 3;
		printf("%s\n", s);
	}

	return 0;
}

int parse_url(const char *url)
{
	get_hostname(url);
	return 0;
}

// https://images-ext-1.discordapp.net/.../.../.../.../.../interestellar-cry.mp4
