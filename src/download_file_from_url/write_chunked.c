#include "download_file_from_url.h"
#include "tools.h"
#include <stdlib.h>

static int	concatenate_buffers(char *dest, char *prev_buf, int prev_buf_len,
											char *cur_buf, int cur_buf_len)
{
	if (!dest)
		return 0;
	if (!prev_buf && !cur_buf)
		return 0;
	if (prev_buf && prev_buf_len > 0)
		memcpy(dest, prev_buf, prev_buf_len);
	else
		prev_buf_len = 0;
	if (cur_buf && cur_buf_len > 0)
		memcpy(dest + prev_buf_len, cur_buf, cur_buf_len);
	return prev_buf_len + cur_buf_len;

}

static int	locate_chunk_size_beacon(char *buf, int pos, int size)
{
	int	i;

	if (!buf)
		return 0;
	i = pos;
	while (i < size) {
		if (i + 1 < size && buf[i] == '\r' && buf[i + 1] == '\n')
			return i;
		i++;
	}
	return i;
}

/**
 * @brief start and end are included
 */
static void	delete_part_of_buf(char *concat_buf, int *concat_buf_len,
								int *prev_buf_len, int start, int end)
{
	int	trash_part_len;

	if (end < start || !concat_buf || *concat_buf_len <= 0)
		return;
	trash_part_len = end - start + 1;
	if (start < *prev_buf_len) { // if start is in prev_buf
		if (start + trash_part_len >= *prev_buf_len)
			*prev_buf_len = start; // end in cur_buf or nothing left in prev_buf
		else
			*prev_buf_len -= trash_part_len; // something left in prev_buf
	}
	// redefine len of prev buf if the cut start inside
	if (end + 1 < *concat_buf_len) { // if any content after the cut
		memcpy(concat_buf + start, concat_buf + end + 1,
									*concat_buf_len - (end + 1));
		*concat_buf_len -= trash_part_len;
	} else
		*concat_buf_len = *prev_buf_len;
}

static void	rewrite_buffers(char *prev_buf, int prev_buf_len,
							char *cur_buf, int *cur_buf_len,
							char *concat_buffers, int concat_buffers_len)
{
	if (!concat_buffers || concat_buffers_len == 0)
		return;
	if (prev_buf && prev_buf_len > 0
				&& prev_buf_len <= concat_buffers_len) {
		memcpy(prev_buf, concat_buffers, prev_buf_len);
	}
	*cur_buf_len = concat_buffers_len - prev_buf_len;
	if (cur_buf && *cur_buf_len > 0
				&& *cur_buf_len <= concat_buffers_len - prev_buf_len) {
		memcpy(cur_buf, concat_buffers + prev_buf_len, *cur_buf_len);
	}
}

void	remove_chunk_format(char *prev_buf, int *prev_buf_len,
							char *cur_buf, int *cur_buf_len, bool *is_first_buf)
{
	char	concat_buffers[REQUEST_BUFFER_SIZE * 2];
	int		concat_buffers_len;
	int		start;
	int		end;

	concat_buffers_len = concatenate_buffers(concat_buffers,
								prev_buf, *prev_buf_len, cur_buf, *cur_buf_len);
	start = 0;
	end = 0;
	if (*is_first_buf) {
		*is_first_buf = false;
		goto first_buffer;
	}
	// exteption for the first chunk size that has no \r\n before
	while ((start = locate_chunk_size_beacon(concat_buffers,
							start, concat_buffers_len)) < concat_buffers_len) {
		end = start + 2; // ignore the first \r and \n found
	first_buffer:
		while (end < concat_buffers_len && concat_buffers[end] != '\r')
			end++;
		if (end + 1 < concat_buffers_len && concat_buffers[end] == '\r'
										&& concat_buffers[end + 1] == '\n') {
			delete_part_of_buf(concat_buffers, &concat_buffers_len,
										prev_buf_len, start, end + 1);
		}
		start++;
	}
	rewrite_buffers(prev_buf, *prev_buf_len, cur_buf, cur_buf_len,
											concat_buffers, concat_buffers_len);
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
		if (prev_buf && cur_buf && cur_buf_len > 0)
			memcpy(prev_buf, cur_buf, cur_buf_len);
		*prev_buf_len = cur_buf_len;
		return; // fill the previous buffer and do not write yet
	}
	remove_chunk_format(prev_buf, prev_buf_len,
						cur_buf, &cur_buf_len, is_first_buf);
	if (prev_buf && *prev_buf_len > 0)
		fwrite(prev_buf, 1, *prev_buf_len, dld->fp);
	if (prev_buf && cur_buf && cur_buf_len > 0)
		memcpy(prev_buf, cur_buf, cur_buf_len); // move buffer into previous
	*prev_buf_len = cur_buf_len;
}
