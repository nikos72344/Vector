//
// Created by mikita on 08.04.2021.
//

#ifndef IVECTOR_LOGGERIMPL_H
#define IVECTOR_LOGGERIMPL_H

#include "ILogger.h"
#include <stdio.h>
#include <string>
#include <map>

class LoggerImpl : public ILogger {
public:
    static ILogger *createLogger();

    static ILogger *createLogger(const char *const &filename, bool overwrite = true);

    RC log(RC code, Level level, const char *const &srcfile, const char *const &function, int line);

    RC log(RC code, Level level);

    ~LoggerImpl();

private:
    FILE* stream;
    std::map<RC, std::string> *RCtoString;
    std::map<Level, std::string> *LevelToString;

    static std::map<RC, std::string> *createRCMap(void);

    static std::map<ILogger::Level, std::string> *createLevelMap(void);


    LoggerImpl(const LoggerImpl &);

    LoggerImpl &operator=(const LoggerImpl &);

    LoggerImpl() = default;

};


#endif //IVECTOR_LOGGERIMPL_H
