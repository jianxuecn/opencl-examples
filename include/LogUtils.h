#ifndef __LogUtils_h
#define __LogUtils_h

#include "LogManager.h"
#include <QString>
#include <QByteArray>

inline void log_info(QString const &logmsg) { LOG_INFO(logmsg.toUtf8().constData()); }
inline void log_error(QString const &logmsg) { LOG_ERROR(logmsg.toUtf8().constData()); }
inline void log_warning(QString const &logmsg) { LOG_WARNING(logmsg.toUtf8().constData()); }

#endif
