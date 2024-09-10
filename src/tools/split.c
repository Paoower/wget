#include <stdlib.h>

int	is_charset(const char *str, const char *charset)
{
	int	i;

	i = 0;
	while (charset[i]) {
		if (charset[i] == *str)
			return (1);
		i++;
	}
	return (0);
}

int	nb_splited(const char *str, const char *charset)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (str[i]) {
		if (!is_charset(str + i, charset)) {
			count++;
			while (str[i] && !is_charset(str + i, charset))
				i++;
		}
		while (str[i] && is_charset(str + i, charset))
			i++;
	}
	return (count);
}

char	**allocate_memory(const char *str, const char *charset, int nb_words)
{
	char	**strs;
	int		i;
	int		y;
	int		word_size;

	strs = malloc(sizeof(char *) * (nb_words + 1));
	y = 0;
	i = 0;
	while (str[i]) {
		word_size = 0;
		if (!is_charset(str + i, charset)) {
			while (str[i] && !is_charset(str + i, charset)) {
				word_size++;
				i++;
			}
			strs[y++] = malloc(sizeof (char) * (word_size + 1));
		}
		while (str[i] && is_charset(str + i, charset))
			i++;
	}
	strs[y] = NULL;
	return (strs);
}

char	**split(const char *str, const char *charset)
{
	char	**strs;
	int		i;
	int		y;
	int		c;

	strs = allocate_memory(str, charset, nb_splited(str, charset));
	y = 0;
	i = 0;
	while (str[i]) {
		c = 0;
		if (!is_charset(str + i, charset)) {
			while (str[i] && !is_charset(str + i, charset))
				strs[y][c++] = str[i++];
			strs[y++][c] = '\0';
		}
		while (str[i] && is_charset(str + i, charset))
			i++;
	}
	return (strs);
}
