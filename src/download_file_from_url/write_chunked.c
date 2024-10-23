#include "download_file_from_url.h"
#include "tools.h"
#include <stdlib.h>

static int	concatenate_buffers(char *dest, char *prev_buf, int prev_buf_len,
										char *cur_buf, int cur_buf_len)
{
	memcpy(dest, prev_buf, prev_buf_len);
	memcpy(dest + prev_buf_len + 1, cur_buf, cur_buf_len);
	return prev_buf_len + cur_buf_len;

}

static int	locate_chunk_size_beacon(char *buf, int pos, int size)
{
	int	i;

	i = pos;
	while (i < size && i + 1 < size) {
		if (buf[i] == '\r' && buf[i + 1] == '\n')
			return i;
		i++;
	}
	return i;
}

/**
 * @brief start included, end not included
 */
static void	delete_part_of_buf(char *concat_buf, int *concat_buf_len,
								int *prev_buf_len, int start, int end)
{
	int	trash_part_len;

	if (start < *prev_buf_len)
		*prev_buf_len = start;
	memmove(concat_buf + start, concat_buf + end, *concat_buf_len - end);
	trash_part_len = end - start;
	*concat_buf_len -= trash_part_len;
}

static void	remove_chunk_format(char *prev_buf, int *prev_buf_len,
							char *cur_buf, int *cur_buf_len, bool is_first_buf)
{
	char	concat_buffers[REQUEST_BUFFER_SIZE * 2];
	int		concat_buffers_len;
	int		start;
	int		end;

	concat_buffers_len = concatenate_buffers(concat_buffers,
								prev_buf, *prev_buf_len, cur_buf, *cur_buf_len);
	start = 0;
	if (is_first_buf)
		goto first_buffer;
	// exteption for the first chunk size that has no \r\n before
	while ((start = locate_chunk_size_beacon(concat_buffers,
							start, concat_buffers_len)) < concat_buffers_len) {
		start += 2; // ignore the first \r and \n
first_buffer:
		end = start;
		while (end < concat_buffers_len && is_char_hexa(concat_buffers[end]))
			end++;
		if (concat_buffers[end] == '\r' && concat_buffers[end + 1] == '\n')
			delete_part_of_buf(concat_buffers, &concat_buffers_len, prev_buf_len,
														start, end + 2);
	}
}

/**
 * @brief Concat two buffers, trim chunk format
 * and write the previous trimed buffer
 */
void	write_chunked(struct dl_data *dld, char *prev_buf, int *prev_buf_len,
										char *cur_buf, int cur_buf_len,
										bool *is_first_read, bool *is_first_buf)
{
	if (*is_first_read) {
		*is_first_read = false;
		memcpy(prev_buf, cur_buf, cur_buf_len);
		*prev_buf_len = cur_buf_len;
		return;
	}
	remove_chunk_format(prev_buf, prev_buf_len,
						cur_buf, &cur_buf_len, *is_first_buf);
	fwrite(prev_buf, 1, *prev_buf_len, dld->fp);
	memcpy(prev_buf, cur_buf, cur_buf_len);
	*prev_buf_len = cur_buf_len;
	if (*is_first_buf)
		*is_first_buf = false;
	// move buffer into previous
}
