#include "Logger.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace exco {

Logger* Logger::mpLogger = nullptr;

void Logger::Write(const char* pFormat, ...)
{
	va_list argp;
	va_start(argp, pFormat);
	getInstance()->write(pFormat, argp);
	va_end(argp);
}

void Logger::Whitespace(unsigned int lines)
{
	getInstance()->whitespace(lines);
}

Logger::Logger()
{
	memset(mBuffer, 0, sizeof(mBuffer));

	fopen_s(&mpFile, "Buzzsaw_Logger.log", "w");
}

Logger::~Logger()
{
	if (mpFile)
	{
		fclose(mpFile);
		mpFile = nullptr;
	}
}

Logger* Logger::getInstance()
{
	if (!mpLogger)
		mpLogger = new Logger();

	return mpLogger;
}

void Logger::write(const char* pFormat, va_list& argp)
{
	memset(mBuffer, 0, sizeof(mBuffer));

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "(%d-%m-%Y|%H:%M:%S)");
	auto strTime = oss.str();

	vsprintf_s(mBuffer, pFormat, argp);

	std::cout << strTime << ": " << mBuffer << std::endl;

	if (mpFile)
		fprintf_s(mpFile, "%s: %s\n", strTime.c_str(), mBuffer);
}

void Logger::whitespace(unsigned int lines)
{
	auto do_work = [&]()
	{
		std::cout << mBuffer << std::endl;

		if (mpFile)
			fprintf_s(mpFile, "%s", mBuffer);
	};

	unsigned int heaps = lines / mBufferSize;
	unsigned int remainder = lines % mBufferSize;

	if (heaps > 0)
	{
		memset(mBuffer, '\n', sizeof(mBuffer));
		for (unsigned int heap = 0; heap < heaps; ++heap)
			do_work();
	}
	
	if (remainder > 0)
	{
		memset(mBuffer, '\n', remainder);
		memset(mBuffer + remainder, 0, mBufferSize - remainder);
		do_work();
	}
}

} // namespace exco