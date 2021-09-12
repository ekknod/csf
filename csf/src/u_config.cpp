#include "../include/u_config.h"
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int u_config_table::GetInt(const char *name)
{
	char b[32];
	GetVal(name, b);
	return atoi(b);
}

float u_config_table::GetFloat(const char *name)
{
	char b[32];
	GetVal(name, b);
	return atof(b);
}

bool u_config_table::SetPos(const char *name)
{
	const char *n;
	char c;

	n = name;
	while (1)
	{
		if (read(_fd, &c, sizeof(c)) == 0)
			break;
		if (c == '}')
			break;
		if (*n != c)
		{
			n = name;
			continue;
		}
		if (*(char *)(n + 1) == '\0')
		{
			lseek(_fd, 2, SEEK_CUR);
			return true;
		}
		n++;
	}
	return false;
}

bool u_config_table::GetVal(const char *name, char buffer[32])
{
	uintptr_t c;
	bool s;
	int i;

	c = lseek(_fd, 0, SEEK_CUR);
	if ((s = SetPos(name)) == false)
	{
		goto end;
	}
	i = 0;
	while (i < 32)
	{
		if (read(_fd, &buffer[i], 1) == 0)
			break;
		if (buffer[i] == '\n')
		{
			buffer[i] = '\0';
			s = true;
			break;
		}
		i++;
	}
end:
	lseek(_fd, c, SEEK_SET);
	return s;
}

u_config::u_config(const char *path) : _path(path) {}
u_config::~u_config(void) {}

bool u_config::exists(void)
{
	return access(_path, F_OK) != -1;
}

void u_config::open_config(void)
{
	_fd = open(_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
}

void u_config::close_config(void)
{
	close(_fd);
}

void u_config::write_config(void *buffer, size_t length)
{
	write(_fd, buffer, length);
}

u_config_table u_config::table(const char *name)
{
	const char *n;
	char c;

	lseek(_fd, 0, SEEK_SET);
	n = name;
	while (1)
	{
		if (read(_fd, &c, sizeof(c)) == 0)
			break;
		if (*n != c)
		{
			n = name;
			continue;
		}
		if (*(char *)(n + 1) == 0)
		{
			return *(u_config_table *)&_fd;
		}
		n++;
	}
	throw name;
}
