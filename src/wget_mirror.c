#include "src.h"
#include "tools.h"
#include "download_file_from_url.h"

void	clean_files_data(struct file_data ***files_data)
{
	int	i;

	if (!files_data || !*files_data)
		return;
	i = 0;
	while ((*files_data)[i])
		free_file_data((*files_data)[i++]);
	free(*files_data);
	*files_data = NULL;
}

int	wget_mirror(char *url, struct parameters_t params)
{
	arraystr			urls;
	arraystr			new_urls;
	arraystr			dl_history;
	struct file_data	**files_data;
	struct file_data	*file_data;
	int					i;

	urls = arraystr_init(url, NULL);
	dl_history = NULL;
	files_data = NULL;
	while (urls) {
		i = 0;
		while (urls[i]) {
			if (!is_in_arraystr(dl_history, urls[i])) {
			// if url is not in the dl_history, dl + add
				file_data = download_file_from_url(urls[i],
									params.storage_path, params.output_file,
									params.rate_limit, params.mirror, 0);
				if (file_data) {
					array_append((void ***)&files_data, (void *)file_data);
					arraystr_append(&dl_history, urls[i]);
				}
			}
			i++;
		}
		clean_arraystr(&urls);
		i = 0;
		while (files_data && files_data[i]) { // TODO: And is HTML
			new_urls = parse_links_from_html(files_data[i], params.reject_list,
					params.exclude_list, params.convert_links, params.mirror);
			arraystr_merge(&urls, new_urls);
			clean_arraystr(&new_urls);
			// concatenate every urls in every files

			// TEMPORARY STOP TO AVOID SEG FAULT
			print_arraystr(urls, "ulrs");
			return 0;
			////////////////////////////////////
		}
		clean_files_data(&files_data);
		arraystr_deduplicate(&urls);
		// remove every duplicated in urls
	}
	clean_arraystr(&dl_history);
	return 0;
}
