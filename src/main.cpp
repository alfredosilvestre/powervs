#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QSplashScreen>

extern "C"
{
    #include <libavformat/avformat.h>
}

#ifdef PORTAUDIO
#include <portaudio.h>
#endif

#include <log4cxx/logger.h>
#include "log4cxx/xml/domconfigurator.h"

using namespace log4cxx;

LoggerPtr logger(Logger::getLogger("Main"));

MainWindow* form = NULL;

void avlogCallback(void* ptr, int level, const char* fmt, va_list v1)
{
    if (level > AV_LOG_WARNING)
        return;

    if(ptr != NULL && fmt != NULL && v1 != NULL)
    {
        // Needed because the Windows runtime aborts and the application crashes if it finds %t or %z.
        QString format = QString(fmt).replace("%t", "%").replace("%z", "%");

        char message[8192];
        char message1[8192];
        const char* module = (*(AVClass**)ptr)->class_name;
        vsnprintf_s(message, sizeof(message), format.toStdString().c_str(), v1);
        sprintf_s(message1, "%s: %s", module, message);

        if(level == AV_LOG_WARNING)
        {
            LOG4CXX_WARN(Logger::getLogger("FFError"), QString(message1).toStdString());
        }
        else LOG4CXX_ERROR(Logger::getLogger("FFError"), QString(message1).toStdString());

        if(form != NULL)
            form->checkFFError((int)ptr, QString(message1));
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QString path = QApplication::applicationDirPath() + "/logs/";
    QDir().mkpath(path);
    path += "powervslog " + QDateTime::currentDateTime().toString("yyyy-MM-dd HHmmss") + ".log";

    qputenv("LOGFILENAME", path.toUtf8());
    xml::DOMConfigurator::configure("Log4cxxConfig.xml");

    LOG4CXX_INFO(logger, "Application started");

    QSplashScreen splash(QPixmap(":/images/splash.jpg"));
    splash.show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    // Initialize libraries
    av_register_all();
    avfilter_register_all();
    av_log_set_callback(avlogCallback);
#ifdef PORTAUDIO
    Pa_Initialize();
#endif

    form = new MainWindow();
    splash.finish(form);
    form->showFullScreen();

    LOG4CXX_INFO(logger, "Interface loaded");

    int ret_val = app.exec();

    // Deinitialize libraries
#ifdef PORTAUDIO
    Pa_Terminate();
#endif

    LOG4CXX_INFO(logger, "Application closed correctly");

    return ret_val;
}
