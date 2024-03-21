#pragma once
#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <vector>
#include <string>

namespace Glb {

    class Logger {
    public:

        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }


        // 添加一条日志消息到缓冲区
        void addLog(const std::string& message) {
            logBuffer.push_back(message);
        }

        // 清空日志缓冲区
        void clearLog() {
            logBuffer.clear();
        }

        std::vector<std::string>& getLog() {
            return logBuffer;
        }

    private:
        Logger() {

        };
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::vector<std::string> logBuffer; // 日志缓冲区
    };

}
#endif