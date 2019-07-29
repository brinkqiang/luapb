
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __DMLOG_H__
#define __DMLOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>
#include "dmutil.h"
#include "dmformat.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define LOG_CRITICAL(...) CDMLog::Instance()->GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) CDMLog::Instance()->GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)  CDMLog::Instance()->GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)  CDMLog::Instance()->GetLogger()->info(__VA_ARGS__)
#define LOG_DEBUG(...) CDMLog::Instance()->GetLogger()->debug(__VA_ARGS__)
#define LOG_TRACE(...) CDMLog::Instance()->GetLogger()->trace(__VA_ARGS__)

class CDMLog
{
public:
    CDMLog()
    {
        std::string strPath = DMGetRootPath() + "/logs";
        std::string strFile = DMGetRootPath() + "/logs/" + DMGetExeName() + ".log";
        DMCreateDirectories((DMGetRootPath() + "/logs").c_str());

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S %f] [%t][%l] %v");

        auto daily_logger = std::make_shared<spdlog::sinks::daily_file_sink_mt>(strFile, 2, 30);
        daily_logger->set_level(spdlog::level::trace);
        daily_logger->set_pattern("[%Y-%m-%d %H:%M:%S %f] [%t][%l] %v");
        spdlog::logger logger(DMGetExeName(), { console_sink, daily_logger });
        logger.set_level(spdlog::level::trace);
        my_logger = logger.clone(DMGetExeName());
    }
    ~CDMLog()
    {

    }

    static CDMLog* Instance() {
        static CDMLog s_oT;
        return &s_oT;
    }

    inline std::shared_ptr<spdlog::logger> GetLogger() { return my_logger; }

private:
    std::shared_ptr<spdlog::logger> my_logger;

};

#define DMLOG_INIT() CDMLog::Instance()

#endif // __DMLOG_H__