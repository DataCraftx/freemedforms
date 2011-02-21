/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef TKLOG_H
#define TKLOG_H

// include toolkit headers
#include <utils/global_exporter.h>
class tkLogPrivate;

// include Qt headers
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMultiMap>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QObject>
#include <QDebug>
class QTreeWidget;


/**
 * \file log.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 05 Feb 2011
*/

#define LOG_ERROR_FOR(object, message)  Utils::Log::addError(object, message,__FILE__,__LINE__)
#define LOG_ERROR(message)              Utils::Log::addError(this, message,__FILE__,__LINE__)

#define LOG_QUERY_ERROR_FOR(object, qsqlquery)  Utils::Log::addQueryError(object, qsqlquery,__FILE__,__LINE__)
#define LOG_QUERY_ERROR(qsqlquery)              Utils::Log::addQueryError(this, qsqlquery,__FILE__,__LINE__)

#define LOG_FOR(object, message) Utils::Log::addMessage(object, message)
#define LOG(message) Utils::Log::addMessage(this, message)


namespace Utils {

class UTILS_EXPORT LogData
{
public:
    enum LogDataType {
        Error = 0,
        CriticalError,
        Warning,
        Message
    };

    LogData( const QString &o, const QString &m, const QDateTime &d, const int t ) :
            object(o), message(m), date(d), type(t) {}

    bool isError() const
    {
        if ((type == Error) || (type==CriticalError) || (type==Warning))
            return true;
        return false;
    }
    QString toString() const
    {
        return QString("%1 - %2 : %3").arg(object).arg(date.toString(), message);
    }

    // Datas
    QString object, message;
    QDateTime date;
    int type;
};

class UTILS_EXPORT Log
{

    static void addData( const QString &o, const QString &m, const QDateTime &d, const int t );

public:
    static bool warnPluginsCreation();

    static void muteConsoleWarnings();

    static void addMessage( const QObject * o, const QString & msg, bool forceWarning = false );
    static void addMessage( const QString & object, const QString & msg, bool forceWarning = false );
    static void addMessages( const QObject * o, const QStringList & msg, bool forceWarning = false );
    static void addMessages( const QString &o, const QStringList & msg, bool forceWarning = false );

    static void addError( const QObject * o, const QString & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addError( const QString & object, const QString & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    static void addErrors( const QObject * o, const QStringList & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addErrors( const QString &o, const QStringList & err, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    static void addQueryError( const QObject * o, const QSqlQuery & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );
    static void addQueryError( const QString & o, const QSqlQuery & q, const QString &file = QString::null, const int line = -1, bool forceWarning = false );

    /** \brief Add a message to tkLog containing the elapsed time of \t and restart it. Used for debugging purpose. */
    static void logTimeElapsed( QTime &t, const QString &object, const QString &forDoingThis );

    static bool hasError();

    static QStringList messages();
    static QStringList errors();

    static QString     toString( const QString & settingsLog = QString::null );
    static QString     saveLog( const QString & fileName = QString::null );

    static void messagesToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );
    static void errorsToTreeWidget( QTreeWidget *parent, bool expandedByClass = true );

private:
    static QList<LogData> m_Messages;
    static bool m_HasError;
    static bool m_MuteConsole;
};

}  // end Core
#endif // TKLOG_H
