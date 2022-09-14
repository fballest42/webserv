#pragma once


# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 128
# endif

# include <stdlib.h>
# include <unistd.h>
# include <string.h>

int		get_next_line(int fd, char **line);

