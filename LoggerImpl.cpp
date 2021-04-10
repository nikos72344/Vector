//
// Created by mikita on 08.04.2021.
//

#include "LoggerImpl.h"

std::map<RC, std::string> *LoggerImpl::createRCMap() {
    std::map<RC, std::string> *newMap = new std::map<RC, std::string>;
    std::string str("UNKNOWN");
    newMap->insert((std::pair<RC, std::string>) {RC::UNKNOWN, str});
    str = "SUCCESS";
    newMap->insert((std::pair<RC, std::string>) {RC::SUCCESS, str});
    str = "INVALID_ARGUMENT";
    newMap->insert((std::pair<RC, std::string>) {RC::INVALID_ARGUMENT, str});
    str = "MISMATCHING_DIMENSIONS";
    newMap->insert((std::pair<RC, std::string>) {RC::MISMATCHING_DIMENSIONS, str});
    str = "INDEX_OUT_OF_BOUND";
    newMap->insert((std::pair<RC, std::string>) {RC::INDEX_OUT_OF_BOUND, str});
    str = "INFINITY_OVERFLOW";
    newMap->insert((std::pair<RC, std::string>) {RC::INFINITY_OVERFLOW, str});
    str = "NOT_NUMBER";
    newMap->insert((std::pair<RC, std::string>) {RC::NOT_NUMBER, str});
    str = "ALLOCATION_ERROR";
    newMap->insert((std::pair<RC, std::string>) {RC::ALLOCATION_ERROR, str});
    str = "NULLPTR_ERROR";
    newMap->insert((std::pair<RC, std::string>) {RC::NULLPTR_ERROR, str});
    str = "FILE_NOT_FOUND";
    newMap->insert((std::pair<RC, std::string>) {RC::FILE_NOT_FOUND, str});
    str = "VECTOR_NOT_FOUND";
    newMap->insert((std::pair<RC, std::string>) {RC::VECTOR_NOT_FOUND, str});
    str = "IO_ERROR";
    newMap->insert((std::pair<RC, std::string>) {RC::IO_ERROR, str});
    str = "MEMORY_INTERSECTION";
    newMap->insert((std::pair<RC, std::string>) {RC::MEMORY_INTERSECTION, str});
    str = "AMOUNT";
    newMap->insert((std::pair<RC, std::string>) {RC::AMOUNT, str});
    return newMap;
}

std::map<ILogger::Level, std::string> *LoggerImpl::createLevelMap() {
    std::map<ILogger::Level, std::string> *newMap = new std::map<ILogger::Level, std::string>;
    std::string str("SEVERE");
    newMap->insert((std::pair<ILogger::Level, std::string>) {Level::SEVERE, str});
    str = "WARNING";
    newMap->insert((std::pair<ILogger::Level, std::string>) {Level::WARNING, str});
    str = "INFO";
    newMap->insert((std::pair<ILogger::Level, std::string>) {Level::INFO, str});
    return newMap;
}

ILogger *LoggerImpl::createLogger() {
    LoggerImpl *newLogger = new LoggerImpl;
    newLogger->stream = stdout;
    newLogger->RCtoString = createRCMap();
    newLogger->LevelToString = createLevelMap();
    return (ILogger *) newLogger;
}

ILogger *LoggerImpl::createLogger(const char *const &filename, bool overwrite) {
    LoggerImpl *newLogger = new LoggerImpl;
    newLogger->stream = fopen(filename, overwrite? "w" : "a");
    newLogger->RCtoString = createRCMap();
    newLogger->LevelToString = createLevelMap();
    return (ILogger *) newLogger;
}

RC LoggerImpl::log(RC code, Level level, const char *const &srcfile, const char *const &function, int line) {
    if (stream == nullptr)
        return RC::IO_ERROR;
    fprintf(stream, "%s %s", LevelToString->operator[](level).data(), RCtoString->operator[](code).data());
    int flag = 1;
    if (srcfile != nullptr) {
        fprintf(stream,": %s", *&srcfile);
        flag = 0;
    }
    if (function != nullptr) {
        if (flag)
            fprintf(stream, ":");
        fprintf(stream, " %s", *&function);
        flag = 0;
    }
    if (line >= 1) {
        if (flag)
            fprintf(stream, ":");
        fprintf(stream, " %i", line);
    }
    fprintf(stream, ";\n");
    return RC::SUCCESS;
}

RC LoggerImpl::log(RC code, Level level) {
    return log(code, level, nullptr, nullptr, 0);
}



LoggerImpl::~LoggerImpl() {
    if(stream!= nullptr && stream!=stdout)
        fclose(stream);
    delete RCtoString;
    delete LevelToString;
}