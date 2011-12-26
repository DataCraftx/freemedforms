/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "genericdescription.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/genericupdateinformation.h>
#include <translationutils/multilingualclasstemplate.h>

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QString>

#include <QDebug>

using namespace Utils;
//using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {

const char * const XML_ROOT_TAG        = "FullDescription";

const char* const ATTRIB_LANG          = "lang";

const char* const TAG_FMF_COMPAT_VERSION   = "fmfv";
const char* const TAG_FD_COMPAT_VERSION   = "fdv";
const char* const TAG_FA_COMPAT_VERSION   = "fav";

const char* const TAG_UUID             = "uuid";
const char* const TAG_VERSION          = "version";
const char* const TAG_WEBLINK          = "weblink";
const char* const TAG_ICON             = "icon";
const char* const TAG_TOOLTIP          = "tooltip";
const char* const TAG_EXTRA            = "extra";
const char* const TAG_AUTHORS          = "authors";
const char* const TAG_VENDOR           = "vendor";
const char* const TAG_VALIDITY         = "validity";
const char* const TAG_CATEGORY         = "category";
const char* const TAG_LICENSE          = "licence";
const char* const TAG_SPECIALTIES      = "specialties";
const char* const TAG_CREATIONDATE     = "cdate";
const char* const TAG_LASTMODIFDATE    = "lmdate";
const char* const TAG_BIBLIOGRAPHY     = "bibliography";
const char* const TAG_DESCRIPTION      = "description";
const char* const TAG_HTMLDESCRIPTION  = "htmldescription";
const char* const TAG_HTMLSYNTHESIS    = "htmlsynthesis";
const char* const TAG_LABEL            = "label";
const char* const TAG_PLUGINNAME       = "type";
const char* const TAG_ABSFILENAME      = "file";

}

/** Create an empty valid object. \e rootTag defines the XML Root Tag for the description block, leave empty to use the default one. */
GenericDescription::GenericDescription(const QString &rootTag)
{
    m_RootTag = rootTag;
    if (m_RootTag.isEmpty())
        m_RootTag = XML_ROOT_TAG;
}

GenericDescription::~GenericDescription()
{
}

/** Define the XML Root Tag for the description block. */
void GenericDescription::setRootTag(const QString &rootTag)
{
    m_RootTag = rootTag;
    if (m_RootTag.isEmpty())
        m_RootTag = XML_ROOT_TAG;
}

/** Clear all data but rootTag */
void GenericDescription::clear()
{
    m_UpdateInfos.clear();
    m_TranslatableExtra.clear();
    m_NonTranslatableExtra.clear();
    m_Data.clear();
}

/** Return the data of the description. \sa addNonTranslatableExtraData(), addTranslatableExtraData() */
QVariant GenericDescription::data(const int ref, const QString &lang) const
{
    QVariant var;
    QString l = lang;
    if (lang.isEmpty()) {
        l = QLocale().name().left(2).toLower();
        if (m_Data.keys().contains(l))
            var = m_Data.value(l).value(ref);
        if (var.isNull()) {
            l = Trans::Constants::ALL_LANGUAGE;
            if (m_Data.keys().contains(l))
                var = m_Data.value(l).value(ref);
        }
    }
    if (var.isNull())
        if (m_Data.keys().contains(l))
            var = m_Data.value(l).value(ref);
    return var;
}

/** Define the data of the description. \sa addNonTranslatableExtraData(), addTranslatableExtraData() */
bool GenericDescription::setData(const int ref, const QVariant &value, const QString &lang)
{
    QString l = lang.toLower();
    if (lang.isEmpty())
        l = Trans::Constants::ALL_LANGUAGE;
    m_Data[l].insert(ref, value);
    return true;
}

/** Populate the description using a xml content \e xmlContent. */
bool GenericDescription::fromXmlContent(const QString &xmlContent)
{
    m_Data.clear();
    if (xmlContent.isEmpty())
        return true;

    QDomDocument doc;
    if (!doc.setContent(xmlContent)) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML");
        return false;
    }
    QDomNodeList els = doc.elementsByTagName(m_RootTag);
    if (els.count() == 0) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML. No root tag: " + m_RootTag);
        return false;
    }
    QDomElement root = els.at(0).toElement();
    return fromDomElement(root);
}

/** Populate the description using a specific local file \e absFileName. */
bool GenericDescription::fromXmlFile(const QString &absFileName)
{
    // Read file content
    QString xml = Utils::readTextFile(absFileName, Utils::DontWarnUser);
    if (xml.isEmpty())
        return false;
    return fromXmlContent(xml);
}

bool GenericDescription::fromDomElement(const QDomElement &root)
{
    if (root.tagName().compare(m_RootTag, Qt::CaseInsensitive)!=0) {
        LOG_ERROR_FOR("GenericDescription", "Wrong XML. No root tag: " + m_RootTag);
        return false;
    }
    QHash<int, QString> elements;
    elements = nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        setData(i.key(), root.firstChildElement(i.value()).text());
    }

    elements = translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        QDomElement desc = root.firstChildElement(i.value());
        while (!desc.isNull()) {
            setData(i.key(), desc.text(), desc.attribute(::ATTRIB_LANG, Trans::Constants::ALL_LANGUAGE));
            desc = desc.nextSiblingElement(i.value());
        }
    }

    // read update informations
    QDomElement update = root.firstChildElement(GenericUpdateInformation::xmlTagName());
    m_UpdateInfos.clear();
    m_UpdateInfos = Utils::GenericUpdateInformation::fromXml(update);
    return true;
}

/** Transform the actual description into XML. */
QString GenericDescription::toXml() const
{
    QDomDocument doc;
    // Create the main description tag
    QDomElement root = doc.createElement(m_RootTag);
    doc.appendChild(root);
    toDomElement(&root, &doc);
    return doc.toString(2);
}

/** Transform the actual description into XML. */
bool GenericDescription::toDomElement(QDomElement *root, QDomDocument *doc) const
{
    Q_ASSERT(root);
    Q_ASSERT(doc);
    if (!root || !doc)
        return false;
    QDomComment comment = doc->createComment("Non translatable values");
    root->appendChild(comment);

    // Set non translatable items
    QHash<int, QString> elements;
    elements = nonTranslatableTagsDataReference();
    QHashIterator<int, QString> i(elements);
    while (i.hasNext()) {
        i.next();
        QDomElement e = doc->createElement(i.value());
        root->appendChild(e);
        e.setNodeValue(data(i.key()).toString());
        QDomText t = doc->createTextNode(data(i.key()).toString());
        e.appendChild(t);
    }

    // Set translatable contents
    comment = doc->createComment("Translatable values");
    root->appendChild(comment);
    elements.clear();
    elements = translatableTagsDataReference();
    i = elements;
    while (i.hasNext()) {
        i.next();
        foreach(const QString &l, m_Data.keys()) {
            const QString &tmp = data(i.key(), l).toString();
            if (tmp.isEmpty())
                continue;
            QDomElement e = doc->createElement(i.value());
            e.setAttribute(ATTRIB_LANG, l);
            root->appendChild(e);
            QDomText t = doc->createTextNode(tmp);
            e.appendChild(t);
        }
    }

    // Add update informations
    if (m_UpdateInfos.count() > 0) {
        comment = doc->createComment("Update information");
        root->appendChild(comment);
        QDomElement e = doc->createElement(GenericUpdateInformation::xmlTagName());
        root->appendChild(e);
        for(int i=0; i < m_UpdateInfos.count(); ++i) {
            m_UpdateInfos.at(i).toDomElement(&e, doc);
        }
    }
    return true;
}

void GenericDescription::addUpdateInformation(const Utils::GenericUpdateInformation &updateInfo)
{
    m_UpdateInfos.append(updateInfo);
    qSort(m_UpdateInfos.begin(), m_UpdateInfos.end(), Utils::GenericUpdateInformation::greaterThan);
}

void GenericDescription::removeUpdateInformation(int index)
{
    if (index >= 0 && index < m_UpdateInfos.count()) {
        m_UpdateInfos.removeAt(index);
    }
}

void GenericDescription::insertUpdateInformation(int index, const Utils::GenericUpdateInformation &updateInfo)
{
    m_UpdateInfos.insert(index, updateInfo);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformation() const
{
    return m_UpdateInfos;
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const QString &version) const
{
    return Utils::GenericUpdateInformation::updateInformationForVersion(m_UpdateInfos, version);
}

QList<Utils::GenericUpdateInformation> GenericDescription::updateInformationForVersion(const Utils::VersionNumber &version) const
{
    return Utils::GenericUpdateInformation::updateInformationForVersion(m_UpdateInfos, version);
}

/** You can add untranslatable extra-data. \e ref must be greated than GenericDescription::NonTranslatableExtraData and lower than GenericDescription::TranslatableExtraData. */
void GenericDescription::addNonTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>NonTranslatableExtraData && ref<TranslatableExtraData);
    if (ref>NonTranslatableExtraData && ref<TranslatableExtraData) {
        m_NonTranslatableExtra.insert(ref, tagName);
    }
}

/** You can add translatable extra-data. \e ref must be greated than GenericDescription::TranslatableExtraData. */
void GenericDescription::addTranslatableExtraData(const int ref, const QString &tagName)
{
    Q_ASSERT(ref>TranslatableExtraData);
    if (ref>TranslatableExtraData) {
        m_TranslatableExtra.insert(ref, tagName);
    }
}

///** Use inheritance to access to unknown QDomElement. This member is called each time a tagName element is unknown to the engine. */
//void GenericDescription::readUnknownElement(const QDomElement &element)
//{
//    // just add a warning
//    qWarning() << Q_FUNC_INFO << "trying to access to an unknown element: tag" << element.tagName();
//    qWarning() << "Use the Utils::GenericDescription inheritance to access to these data";
//}

void GenericDescription::toTreeWidget(QTreeWidget *tree) const
{}

QHash<int, QString> GenericDescription::nonTranslatableTagsDataReference() const
{
    QHash<int, QString> elements;
    // get non translatable items
    elements.insert(GenericDescription::Uuid, TAG_UUID);
    elements.insert(GenericDescription::Version, TAG_VERSION);
    elements.insert(GenericDescription::AbsFileName, TAG_ABSFILENAME);
    elements.insert(GenericDescription::Author, TAG_AUTHORS);
    elements.insert(GenericDescription::Vendor, TAG_VENDOR);
    elements.insert(GenericDescription::Validity, TAG_VALIDITY);
    elements.insert(GenericDescription::FreeMedFormsCompatVersion, TAG_FMF_COMPAT_VERSION);
    elements.insert(GenericDescription::FreeDiamsCompatVersion, TAG_FD_COMPAT_VERSION);
    elements.insert(GenericDescription::FreeAccountCompatVersion, TAG_FA_COMPAT_VERSION);
    elements.insert(GenericDescription::CreationDate, TAG_CREATIONDATE);
    elements.insert(GenericDescription::LastModificationDate, TAG_LASTMODIFDATE);
    elements.insert(GenericDescription::GeneralIcon, TAG_ICON);
    // add extra tags
    QHashIterator<int, QString> i(m_NonTranslatableExtra);
    while (i.hasNext()) {
        i.next();
        elements.insert(i.key(), i.value());
    }
    return elements;
}

QHash<int, QString> GenericDescription::translatableTagsDataReference() const
{
    QHash<int, QString> elements;
    elements.insert(GenericDescription::Label, TAG_LABEL);
    elements.insert(GenericDescription::Category, TAG_CATEGORY);
    elements.insert(GenericDescription::ToolTip, TAG_TOOLTIP);
    elements.insert(GenericDescription::Specialties, TAG_SPECIALTIES);
    elements.insert(GenericDescription::ShortDescription, TAG_DESCRIPTION);
    elements.insert(GenericDescription::HtmlDescription, TAG_HTMLDESCRIPTION);
    elements.insert(GenericDescription::HtmlSynthesis, TAG_HTMLSYNTHESIS);
    elements.insert(GenericDescription::GlobalLicense, TAG_LICENSE);
    elements.insert(GenericDescription::WebLink, TAG_WEBLINK);
    // add extra tags
    QHashIterator<int, QString> i(m_TranslatableExtra);
    while (i.hasNext()) {
        i.next();
        elements.insert(i.key(), i.value());
    }
    return elements;
}
