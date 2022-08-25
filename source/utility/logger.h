#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

namespace exco {

class Logger
{
public:

	static void Write(const char* pFormat, ...);
	static void Whitespace(unsigned int lines);

private:

	Logger();
	~Logger();

	static Logger* getInstance();
	void write(const char* pFormat, va_list& argp);
	void whitespace(unsigned int lines);

private:

	static Logger* mpLogger;

	const static unsigned int mBufferSize = 4096;

	char mBuffer[mBufferSize];
	FILE* mpFile;
};

class ScopeWatch
{
public:
	ScopeWatch(const char* strName)
		: m_strName(strName)
	{ 
		Logger::Write("%s - begin", m_strName);
	}
	
	~ScopeWatch()
	{
		Logger::Write("%s - end", m_strName);
	}

private:

	const char* m_strName;

};

} // namespace exco