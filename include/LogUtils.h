/**
 * -------------------------------------------------------------------------------
 * This source file is part of opencl-examples, which is developed for
 * the Cloud Computing Course and the Computer Graphics Course at the School
 * of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).
 * Copyright (C) 2020-2022 Xue Jian (xuejian@ucas.ac.cn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------------
 */
#ifndef __LogUtils_h
#define __LogUtils_h

#include "LogManager.h"
#include <QString>
#include <QByteArray>

inline void log_info(QString const &logmsg) { LOG_INFO(logmsg.toUtf8().constData()); }
inline void log_error(QString const &logmsg) { LOG_ERROR(logmsg.toUtf8().constData()); }
inline void log_warning(QString const &logmsg) { LOG_WARNING(logmsg.toUtf8().constData()); }

#endif
