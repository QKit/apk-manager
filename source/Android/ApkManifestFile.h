/*******************************************************************************
*                                                                              *
*  Apk manifest file class implementation.                                     *
*                                                                              *
*  Copyright (C) 2012 Kirill Chuvilin.                                         *
*  Contact: Kirill Chuvilin (kirill.chuvilin@gmail.com, kirill.chuvilin.pro)   *
*  The class is based on this Java code: http://pastebin.com/rmc3wR9r          *
*                                                                              *
*  This file is a part of the QKit Apk Manager project.                        *
*  https://github.com/QKit/apk-manager                                         *
*                                                                              *
*  $QT_BEGIN_LICENSE:GPL$                                                      *
*                                                                              *
*  GNU General Public License Usage                                            *
*  This file may be used under the terms of the GNU General Public License     *
*  version 3.0 as published by the Free Software Foundation and appearing in   *
*  the file LICENSE.GPL included in the packaging of this file. Please review  *
*  the following information to ensure the GNU General Public License version  *
*  3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.         *
*                                                                              *
*  This file is distributed in the hope that it will be useful, but WITHOUT    *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
*  more details.                                                               *
*                                                                              *
*  $QT_END_LICENSE$                                                            *
*                                                                              *
*******************************************************************************/

#ifndef _ApkManifestFile_h_
#define _ApkManifestFile_h_

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFile>

#define APK_MANIFEST_END_DOC_TAG 0x00100101
#define APK_MANIFEST_START_TAG   0x00100102
#define APK_MANIFEST_END_TAG     0x00100103

/*!
 * \brief Class with methods to read compressed Android manifest.
 */
class ApkManifestFile : public QObject {
    Q_OBJECT
public:
    /*!
     * \brief Constructor.
     */
    ApkManifestFile() {}

    /*!
     * \brief Constructor.
     * \param binaryData the compressed manifest file binary data
     */
    ApkManifestFile(const QByteArray& binaryData);

    /*!
     * \brief Constructor.
     * \param binaryDevice output device with comressed binary data
     */
    ApkManifestFile(QIODevice& binaryDevice);

    /*!
     * \brief Constructor.
     * \param binaryFilePath string with the path to the compressed manifest file
     */
    ApkManifestFile(const QString& binaryFilePath);

    /*!
     * \brief Get manifest data as text
     * \return string with data
     */
    QString getTextData() const {return this->t_textData;}

    /*!
     * \brief Read a word from the binary data.
     * \return readed Little Endian 16 bit word
     * \param binaryData compressed manifest binary
     * \param offset offset in the data byte array
     */
    static ushort uncompressWord(const QByteArray& binaryData, uint offset) {return (binaryData[offset + 1] << 8 & 0xFF00) | (binaryData[offset] & 0xFF);}

    /*!
     * \brief Read a double word from the binary data.
     * \return readed Little Endian 32 bit word
     * \param binaryData compressed manifest binary
     * \param offset offset in the data byte array
     */
    static ulong uncompressDWord(const QByteArray& binaryData, uint offset) {return (binaryData[offset + 3] << 24&0xFF000000) | (binaryData[offset+2] << 16 & 0xFF0000) | (binaryData[offset + 1] << 8 & 0xFF00) | (binaryData[offset] & 0xFF);}

    /*!
     * \brief Read a string from the binary data.
     * \return readed string
     * \param binaryData compressed manifest binary
     * \param offset offset in the data byte array (starts at 16 bit string length, which is followed by that number of 16 bit (Unicode) chars)
     */
    static QString uncompressString(const QByteArray& binaryData, uint offset);

    /*!
     * \brief Read a string from the string table of the binary data.
     * \return readed string
     * \param binaryData compressed manifest binary
     * \param stringIndex index of the string in the string table
     * \param stringTableOffset offset of the string table in the data byte array
     * \param stringIndexTableOffset offset of the string indexes table in the data byte array
     */
    static QString uncompressString(const QByteArray& binaryData, uint stringIndex, uint stringTableOffset, uint stringIndexTableOffset);

public slots:
    /*!
     * \brief Set data as compressed binary.
     * \param binaryData the compressed manifest file binary data
     */
    void setCompressedBinaryData(const QByteArray& binaryData);

    /*!
     * \brief Set data as input device compressed binary data.
     * \param binaryDevice output device with comressed binary data
     */
    void setCompressedBinaryDevice(QIODevice& binaryDevice);

    /*!
     * \brief Set data as compressed binary file data.
     * \param binaryFilePath string with the path to the compressed manifest file
     */
    void setCompressedBinaryFile(const QString& binaryFilePath);

private:
    QString t_textData; //!< manifext text data
};

#endif // _ApkManifestFile_h_
