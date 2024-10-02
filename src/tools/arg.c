#include <stdarg.h>

int	arg_ptr_len(void *first, va_list args)
{
	int		size;
	void	*ptr;

	ptr = first;
	size = 0;
	while (ptr) {
		size++;
		ptr = va_arg(args, void *);
	}
	return size;
}
