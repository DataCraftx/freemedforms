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

/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \class Views::StringListModel
  * \brief Provides a stringlist model to use with tkStringListView.
  * You can set items checkable and/or string editable with constructor or with setCheckable(), setStringEditable().
  * There are functions to moveUp() moveDown() items.
  * You can set/get checkedItems with getCheckedItems() and setCheckedItems().
  * You can set/get all strings using setStringList().
*/

#include "stringlistmodel.h"

#include <QStringList>
#include <QList>

#include <QDebug>

namespace Views {
namespace Internal {

class StringListModelPrivate
{
public:
    struct Data {
        QString str;
        int     checked;
    };

    bool         m_Checkable;
    bool         m_StringEditable;
    QList<Data>  m_StringList;
};

}  // End Internal
}  // End Views


using namespace Views;
using namespace Views::Internal;

StringListModel::StringListModel( QObject *parent,
                                      const bool stringEditable,
                                      const bool checkStateEditable ) :
    QAbstractListModel(parent), d(new StringListModelPrivate)
{
    setObjectName("StringListModel");
    d->m_Checkable = checkStateEditable;
    d->m_StringEditable = stringEditable;
}

StringListModel::~StringListModel()
{
    if (d) delete d;
    d=0;
}

void StringListModel::setReadOnly(bool state)
{
    setStringEditable(!state);
}

void StringListModel::setCheckable( bool state )
{
    d->m_Checkable = state;
    reset();
}

void StringListModel::setStringEditable( bool state )
{
    d->m_StringEditable = state;
    reset();
}


void StringListModel::setStringList( const QStringList & strings )
{
     d->m_StringList.clear();
     foreach( const QString & s, strings ) {
         StringListModelPrivate::Data dt;
         dt.str = s;
         dt.checked = Qt::Unchecked;
         d->m_StringList << dt;
     }
}

QStringList StringListModel::getStringList() const
{
    QStringList list;
    foreach( const StringListModelPrivate::Data & dt, d->m_StringList )
        list << dt.str;
    return list;
}


QStringList StringListModel::getCheckedItems() const
{
    QStringList list;
    foreach( const StringListModelPrivate::Data & dt, d->m_StringList )
        if ( dt.checked )
            list << dt.str;
    return list;
}

void StringListModel::setCheckedItems( const QStringList & list )
{
    QList<StringListModelPrivate::Data>::iterator i;
    for (i = d->m_StringList.begin(); i != d->m_StringList.end(); ++i) {
       if ( list.indexOf( (*i).str ) != -1 )
           (*i).checked = Qt::Checked;
       else
           (*i).checked = Qt::Unchecked;
    }
    reset();
}

int StringListModel::rowCount( const QModelIndex & ) const
{
    return d->m_StringList.count();
}

Qt::ItemFlags StringListModel::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    if (d->m_Checkable)
        f |= Qt::ItemIsUserCheckable;
    if (d->m_StringEditable)
        f |= Qt::ItemIsEditable;
    return f;
}

bool StringListModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    if ( ! index.isValid() )
        return false;
    if ( index.row() > rowCount() )
        return false;

    if ( ( role == Qt::EditRole ) && (d->m_StringEditable) ){
        d->m_StringList[ index.row() ].str = value.toString();
    } else if ( role == Qt::CheckStateRole ) {
        d->m_StringList[ index.row() ].checked = value.toInt();
    }
    return true;
}

QVariant StringListModel::data( const QModelIndex & index, int role ) const
{
    if ( ! index.isValid() )
        return QVariant();
    if ( index.row() > rowCount() )
        return QVariant();

    if ( role == Qt::CheckStateRole ) {
        return d->m_StringList.at(index.row()).checked;
    } else if ( role == Qt::DisplayRole ) {
        return d->m_StringList.at(index.row()).str;
    } else if ( role == Qt::EditRole ) {
        if (d->m_StringEditable)
            return d->m_StringList.at(index.row()).str;
    }
    return QVariant();
}

bool StringListModel::insertRows ( int row, int count, const QModelIndex & parent )
{
    beginInsertRows(parent,row,row+count);
    int i;
    for(i=0; i<count;++i) {
        StringListModelPrivate::Data data;
        data.str = QString();
        data.checked = Qt::Unchecked;
        d->m_StringList.insert(i+row,data);
    }
    endInsertRows();
    return true;
}

bool StringListModel::removeRows ( int row, int count, const QModelIndex & parent )
{
    beginRemoveRows(parent,row,row+count);
    int i;
    for(i=0; i<count;++i) {
        if ((i+row) < d->m_StringList.count())
            d->m_StringList.removeAt(i+row);
    }
    endRemoveRows();
    return true;
}

bool StringListModel::moveUp( const QModelIndex & item )
{
    if ( !item.isValid() )
        return false;

    if (item.row() >= 1) {
        d->m_StringList.move( item.row(), item.row()-1);
        reset();
        return true;
    }
    return false;
}

bool StringListModel::moveDown( const QModelIndex & item )
{
    if ( !item.isValid() )
        return false;

    if (item.row() < (rowCount()-1)) {
        d->m_StringList.move( item.row(), item.row()+1);
        reset();
        return true;
    }
    return false;
}

