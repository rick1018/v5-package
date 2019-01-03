#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/glog_helper.h"

//将信息输出到单独的文件和 LOG(ERROR)
void SignalHandle(const char* data, int size)
{
    std::string str = std::string(data,size);
    /*
    std::ofstream fs("glog_dump.log",std::ios::app);
    fs<<str;
    fs.close();
    */
    LOG(ERROR) << str;
}

GLogHelper::GLogHelper(const char* program)
{
    struct stat st;
    if (stat(GLOG_FILE_PATH, &st) < 0) {
        if (mkdir(GLOG_FILE_PATH, 0755) < 0) {
            LOG(ERROR) << "create '" << GLOG_FILE_PATH << "' failed: " <<  strerror(errno);
        }
    }

    google::InitGoogleLogging(program);

    google::SetStderrLogging(google::WARNING);                              //设置级别高于 google::INFO 的日志同时输出到屏幕
    FLAGS_colorlogtostderr = true;                                          //设置输出到屏幕的日志显示相应颜色
    google::SetLogDestination(google::INFO, GLOG_FILE_PATH "/LOG-");       //设置 google::INFO 级别的日志存储路径和文件名前缀
    // google::SetLogDestination(google::WARNING, GLOG_FILE_PATH "/WARNING-"); //设置 google::WARNING 级别的日志存储路径和文件名前缀
    // google::SetLogDestination(google::ERROR, GLOG_FILE_PATH "/ERROR-");     //设置 google::ERROR 级别的日志存储路径和文件名前缀
    // google::SetLogDestination(google::FATAL, GLOG_FILE_PATH "/FATAL-");     //设置 google::FATAL 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::WARNING, "");
    google::SetLogDestination(google::ERROR, "");
    google::SetLogDestination(google::FATAL, "");
    FLAGS_logbufsecs = 0;                                                   //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size = 25;                                                //最大日志大小为 25MB
    FLAGS_stop_logging_if_full_disk = true;                                 //当磁盘被写满时，停止日志输出
    google::SetLogFilenameExtension("IPC-");                                //设置文件名扩展，如平台？或其它需要区分的信息
    google::InstallFailureSignalHandler();                                  //捕捉 CoreDump
    google::InstallFailureWriter(&SignalHandle);                            //默认捕捉 SIGSEGV 信号信息输出会输出到 stderr，可以通过下面的方法自定义输出方式：
}

GLogHelper::~GLogHelper()
{
    google::ShutdownGoogleLogging();
}
