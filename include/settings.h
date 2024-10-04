#ifndef SETTINGS_H
# define SETTINGS_H

# include "stddef.h"

# define URL_REGEX "^((http|https)://)?(www\\.)?.*\\.[a-z].*?$"
# define FILE_PATH_REGEX "^(\\.?\\/)?([a-zA-Z0-9_-]+\\/)*[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9]+)?$"
# define DATA_PATH "./data/"

const char	*LINK_ATTRIBUTES[] = {
	"src",
	"href",
	"action",
	"formaction",
	"cite",
	"data",
	"usemap",
	"codebase",
	"manifest",
	"poster",
	"icon",
	"profile",
	"archive",
	"longdesc",
	"ping",
	"srcset",
	"integrity",
	"background",
	"srcdoc",
	"content",
	NULL
};

#endif
