/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "serializer.h"
#include <utils/log.h>

#include <QApplication>
#include <QLatin1Char>
#include <QVariant>
#include <QRect>
#include <QPoint>

using namespace Utils;
using namespace Utils::Constants;

namespace Utils {
namespace Serializer {

/**
  \brief Official separator for the serializer engine.
*/
const QString separator()
{
    return QString(SERIALIZER_SEPARATOR);
}
const QString openParenthese()
{
    return QString("#(");
}
const QString closeParenthese()
{
    return QString(")#");
}

QStringList splitArgs(const QString &s, int idx)
{
    int l = s.length();
    Q_ASSERT(l > 0);
    Q_ASSERT(s.at(idx) == QLatin1Char('('));
    Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));
    QStringList result;
    QString item;
    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            Q_ASSERT(idx == l - 1);
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }
    return result;
}

QString variantToString(const QVariant &v)
{
    QString result;
    switch (v.type()) {
        case QVariant::Invalid:
            result = QLatin1String("@Invalid()");
            break;
        case QVariant::ByteArray: {
            QByteArray a = v.toByteArray();
            result = QLatin1String("@ByteArray(");
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
            break;
        }

    case QVariant::String:
    case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::Bool:
        case QVariant::Double:
        case QVariant::KeySequence: {
            result = v.toString();
            if (result.startsWith(QLatin1Char('@')))
                result.prepend(QLatin1Char('@'));
            break;
        }
#ifndef QT_NO_GEOM_VARIANT
        case QVariant::Rect: {
            QRect r = qvariant_cast<QRect>(v);
            result += QLatin1String("@Rect(");
            result += QString::number(r.x());
            result += QLatin1Char(' ');
            result += QString::number(r.y());
            result += QLatin1Char(' ');
            result += QString::number(r.width());
            result += QLatin1Char(' ');
            result += QString::number(r.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Size: {
            QSize s = qvariant_cast<QSize>(v);
            result += QLatin1String("@Size(");
            result += QString::number(s.width());
            result += QLatin1Char(' ');
            result += QString::number(s.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Point: {
            QPoint p = qvariant_cast<QPoint>(v);
            result += QLatin1String("@Point(");
            result += QString::number(p.x());
            result += QLatin1Char(' ');
            result += QString::number(p.y());
            result += QLatin1Char(')');
            break;
        }
#endif // !QT_NO_GEOM_VARIANT

    default: {
#ifndef QT_NO_DATASTREAM
            QByteArray a;
            {
                QDataStream s(&a, QIODevice::WriteOnly);
                s.setVersion(QDataStream::Qt_4_0);
                s << v;
            }

            result = QLatin1String("@Variant(");
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
#else
            Q_ASSERT(!"QSettings: Cannot save custom types without QDataStream support");
#endif
            break;
        }
    }
    return result;
}

QVariant stringToVariant(const QString &s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(s.toLatin1().mid(11, s.size() - 12));
            } else if (s.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
                QByteArray a(s.toLatin1().mid(9));
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_0);
                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT(!"QSettings: Cannot load custom types without QDataStream support");
#endif
#ifndef QT_NO_GEOM_VARIANT
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = splitArgs(s, 6);
                if (args.size() == 2)
                    return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
#endif
            } else if (s == QLatin1String("@Invalid()")) {
                return QVariant();
    }
        }
        if (s.startsWith(QLatin1String("@@")))
            return QVariant(s.mid(1));
    }
    return QVariant(s);
}


/**
  \brief Serialize a QStringList into a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QString toString( const QStringList &list, bool base64Protection )
{
    if (!base64Protection)
        return list.join( Serializer::separator() );
    QString tmp;
    foreach( const QString & s, list ) {
        tmp += s.toAscii().toBase64() + Serializer::separator();
    }
    if (!tmp.isEmpty())
        tmp.chop(Serializer::separator().length());
    return tmp;
}


/**
  \brief Deserialize a QStringList from a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QStringList toStringList( const QString &serialized, bool base64Protection )
{
    if (!base64Protection)
        return serialized.split( Serializer::separator() );
    QStringList toReturn;
    foreach( const QString &s, serialized.split(Serializer::separator()) ) {
        toReturn << QByteArray::fromBase64(s.toAscii());
    }
    return toReturn;
}

/**
  \brief Serialize a QHash<int, QString> into a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QString toString( const QHash<int,QString> &hash, bool base64Protection )
{
    QString tmp;
    tmp = HASH_PREFIX;
    if (!base64Protection) {
        // Add key + Separator + value + Separator
        foreach( int i, hash.keys() ) {
            tmp += QString::number(i) + separator() + hash.value(i) + separator();
        }
        //        tmp << hash;
        return tmp;
    }
    QString val;
    foreach( int i, hash.keys() ) {
        val = hash.value(i);
        tmp += QString::number(i) + separator() + val.toAscii().toBase64() + separator();
    }
    return tmp;
}

/**
  \brief Deserialize a QHash<int, QString> from a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QHash<int,QString> toHash( const QString & serialized, bool base64Protection )
{
    QHash<int,QString> toReturn;
    if (!serialized.startsWith(HASH_PREFIX)) {
        Log::addError( "Serializer", QCoreApplication::translate("Serializer", "Can not deserialize to QHash.") );
        return toReturn;
    }
    QString ser = serialized.mid( QString(HASH_PREFIX).length() );
    QStringList list = ser.split( separator() );
    QStringListIterator it(list);
    QString val;
    QString k;
    if (base64Protection) {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val.toInt(), QByteArray::fromBase64( k.toAscii() ) );
            }
        }
    } else {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val.toInt(), k );
            }
        }
    }
    return toReturn;
}

/**
  \brief Serialize a QHash<int, QVariant> into a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QString toString(const QHash<int,QVariant> &hash, bool base64Protection)
{
    QString tmp = HASH_PREFIX;
    if (!base64Protection) {
        // Add key + Separator + value + Separator
        foreach( int i, hash.keys() ) {
            tmp += openParenthese() + QString::number(i) + separator() + variantToString(hash.value(i)) + closeParenthese();
        }
        //        tmp << hash;
        return tmp;
    }
    QString val;
    foreach( int i, hash.keys() ) {
        val = variantToString(hash.value(i));
        tmp += openParenthese() + QString::number(i) + separator() + val.toAscii().toBase64() + closeParenthese();
    }
    return tmp;
}

/**
  \brief Deserialize a QHash<int, QVariant> from a simple QString.
  You can protect the serialization/deserialization with base64 encoding. In this case, datas from the
  object to serialize/deserialize are encoded/decoded from base64.
*/
const QHash<int,QVariant> toVariantHash( const QString &serialized, bool base64Protection )
{
    QHash<int,QVariant> toReturn;
    if (!serialized.startsWith(HASH_PREFIX)) {
        Log::addError( "Serializer", QCoreApplication::translate("Serializer", "Can not deserialize to QHash.") );
        return toReturn;
    }
    QString ser = serialized.mid(QString(HASH_PREFIX).length());
    QStringList list = ser.split(openParenthese(), QString::SkipEmptyParts);
    QString val;
    if (base64Protection) {
        foreach(const QString s, list) {
            QStringList content = s.split(separator(), QString::KeepEmptyParts);
            val = content.at(1);
            val = val.remove(closeParenthese());
            toReturn.insert(content.at(0).toInt(), stringToVariant(QByteArray::fromBase64(val.toAscii())));
        }
    } else {
        foreach(const QString s, list) {
            QStringList content = s.split(separator(), QString::KeepEmptyParts);
            val = content.at(1);
            val = val.remove(closeParenthese());
            toReturn.insert(content.at(0).toInt(), stringToVariant(val.toAscii()));
        }
    }
    return toReturn;
}


/**
  \brief obsolete
  \obsolete
*/
const QString threeCharKeyHashToString( const QHash<QString,QString> & hash, bool base64Protection )
{
    QString tmp;
    tmp = HASH_PREFIX;
    if (!base64Protection) {
        // Add key + Separator + value + Separator
        foreach( const QString &i, hash.keys() ) {
            tmp += i + separator() + hash.value(i) + separator();
        }
        //        tmp << hash;
        return tmp;
    } else {
        QString val;
        foreach( const QString &i, hash.keys() ) {
            val = hash.value(i);
            tmp += i + separator() + val.toAscii().toBase64() + separator();
        }
    }
    return tmp;
}

/**
  \brief obsolete
  \obsolete
*/
const QHash<QString,QString> threeCharKeyHashToHash( const QString & serialized, bool base64Protection )
{
    QHash<QString,QString> toReturn;
    if (!serialized.startsWith(HASH_PREFIX)) {
        Log::addError( "Serializer", QCoreApplication::translate("Serializer", "Can not deserialize to QHash.") );
        return toReturn;
    }
    QString ser = serialized.mid( QString(HASH_PREFIX).length() );
    QStringList list = ser.split( separator() );
    QStringListIterator it(list);
    QString val;
    QString k;
    if (base64Protection) {
        while (it.hasNext()) {
            val = it.next();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val, QByteArray::fromBase64( k.toAscii() ) );
            }
        }
    } else {
        while (it.hasNext()) {
            val = it.next().toAscii();
            if (it.hasNext()) {
                k = it.next();
                toReturn.insert( val, k );
            }
        }
    }
    return toReturn;
}


}  // End Serializer
}  // End Utils
