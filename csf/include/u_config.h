#ifndef U_CONFIG_H
#define U_CONFIG_H

typedef __SIZE_TYPE__ size_t;

class u_config_table
{
	int _fd;

public:
	int GetInt(const char *name);
	float GetFloat(const char *name);

private:
	bool SetPos(const char *name);
	bool GetVal(const char *name, char buffer[32]);
};

class u_config
{
	const char *_path;
	int _fd;

public:
	u_config(const char *path);
	~u_config(void);
	bool exists(void);
	void open_config(void);
	void close_config(void);
	u_config_table table(const char *name);
	void write_config(void *buffer, size_t length);
};

#endif // U_CONFIG_H
