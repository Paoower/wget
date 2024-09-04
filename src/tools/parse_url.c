#include "tools.h"
#include "args.h"

int get_hostname()
{
    char *s = strstr(params.url, "://");
    
    if (s) {
        s += 3;
    }
}

int parse_url()
{
    get_hostname();
    return 0;
}

// https://images-ext-1.discordapp.net/.../.../.../.../.../interestellar-cry.mp4