#include <stdbool.h>

bool	is_char_hexa(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
									|| (c >= '0' && c <= '9');
}
