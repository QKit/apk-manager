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

#include "ApkManifestFile.h"

ApkManifestFile::ApkManifestFile(const QByteArray& binaryData) {
    this->setCompressedBinaryData(binaryData);
}


ApkManifestFile::ApkManifestFile(QIODevice& binaryDevice) {
    this->setCompressedBinaryDevice(binaryDevice);
}


ApkManifestFile::ApkManifestFile(const QString& binaryFilePath) {
    this->setCompressedBinaryFile(binaryFilePath);
}


QString ApkManifestFile::uncompressString(const QByteArray& binaryData, uint offset) {
    QString string; // result string
    ushort length = ApkManifestFile::uncompressWord(binaryData, offset); // string length
    for (ushort charOffset = offset + 2; charOffset < offset + 2 + 2 * length; charOffset += 2) { // for all chars
        string += QChar(ApkManifestFile::uncompressWord(binaryData, charOffset)); // read next char
    }
    return string;
}


QString ApkManifestFile::uncompressString(const QByteArray& binaryData, uint stringIndex, uint stringTableOffset, uint stringIndexTableOffset) {
    if (stringIndex < 0) return QString::null;
    uint offset = stringTableOffset + ApkManifestFile::uncompressDWord(binaryData, stringIndexTableOffset + stringIndex * 4); // offset of the string in data byte array
    return ApkManifestFile::uncompressString(binaryData, offset);
}


void ApkManifestFile::setCompressedBinaryData(const QByteArray& binaryData) {
    this->t_textData.clear(); // delete existing data

    // Compressed XML bytes starts with 24 bytes of data, 6 32 bit words in little endian order (LSB first):
    //   0th word is 03 00 08 00
    //   3rd word SEEMS TO BE:  Offset at then of StringTable
    //   4th word is: Number of strings in string table

    ulong nStrings = ApkManifestFile::uncompressDWord(binaryData, 0x10); // number of strings in the table
    uint stringIndexTableOffset = 0x24;  // offset of the string indexes table
    uint stringTableOffset = stringIndexTableOffset + nStrings * 4;  // ofset of the strings table

    // The XML tag tree starts after some unknown content after the strings table.
    // There is some unknown data after the string table, scan forward from this point to the flag for the start of an XML start tag.

    ulong xmlTagsOffset = ApkManifestFile::uncompressDWord(binaryData, 3 * 0x4);  // start from the offset in the 3rd word
    for (uint testOffset = xmlTagsOffset; testOffset + 4 < (uint)binaryData.length(); testOffset += 4) { // scan through the byte array
        if (ApkManifestFile::uncompressDWord(binaryData, testOffset) == APK_MANIFEST_START_TAG) { // if there is start tag flag
            xmlTagsOffset = testOffset; // go to these point
            break;
        }
    }

    // XML tags and attributes:
    // Every XML start and end tag consists of 6 32 bit words:
    //   0th word: 02011000 for APK_MANIFEST_START_TAG and 03011000 for APK_MANIFEST_END_TAG
    //   1st word: a flag?, like 38000000
    //   2nd word: a line of where this tag appeared in the original source file
    //   3rd word: FFFFFFFF ??
    //   4th word: string index of namespace name, or FFFFFFFF for default namespace
    //   5th word: string index of element name
    //   (Note: 01011000 in 0th word means end of XML document, APK_MANIFEST_END_DOC_TAG)

    // Start tags (not end tags) contain 3 more words:
    //   6th word: 14001400 meaning??
    //   7th word: number of attributes that follow this tag (follow word 8th)
    //   8th word: 00000000 meaning??

    // Attributes consist of 5 words:
    //   0th word: string index of attribute name's namespace, or FFFFFFFF
    //   1st word: string index of attribute name
    //   2nd word: string index of attribute value, or FFFFFFF if ResourceId used
    //   3rd word: Flags?
    //   4th word: str ind of attr value again, or ResourceId of value

    for (uint offset = xmlTagsOffset; offset < (uint)binaryData.length(); ) {
        ulong tag = ApkManifestFile::uncompressDWord(binaryData, offset);
        // todo: dataDWord(data, offset + 1 * 0x4)
        // ulong lineNumber = dataDWord(data, offset + 2 * 0x4);
        // todo: dataDWord(data, offset + 3 * 0x4)
        // ulong nameNamespaceStringIndex = dataDWord(data, offset + 4 * 0x4);
        ulong nameStringIndex = ApkManifestFile::uncompressDWord(binaryData, offset + 5 * 0x4);
        switch (tag) {
        case APK_MANIFEST_START_TAG: { // XML START TAG
            // todo: dataDWord(data, offset + 6 * 0x4) - expected to be 14001400
            ulong attributesCount = ApkManifestFile::uncompressDWord(binaryData, offset + 7 * 0x4); // number of attributes to follow
            // todo: dataDWord(data, offset + 8 * 0x4) - expected to be 00000000
            offset += 9 * 0x4; // skip APK_MANIFEST_START_TAG data
            QString tagName = ApkManifestFile::uncompressString(binaryData, nameStringIndex, stringTableOffset, stringIndexTableOffset); // name of the tag
            QString tagAttributes; // string with tag attibutes
            for (ulong iAttribute = 0; iAttribute < attributesCount; iAttribute++) {
                // ulong attributeNameNamespaceStringIndex = dataDWord(data, offset);  // attribute name namespace string index or FFFFFFFF
                ulong attributeNameStringIndex = ApkManifestFile::uncompressDWord(binaryData, offset + 1 * 0x4);  // attribute name string index
                ulong attributeValueStringIndex = ApkManifestFile::uncompressDWord(binaryData, offset + 2 * 0x4); // attribute value string index or FFFFFFFF
                // todo: dataDWord(data, offset + 3 * 0x4);
                ulong attrResId = ApkManifestFile::uncompressDWord(binaryData, offset + 4 * 0x4);  // attribute value ResourceId or dup attribute value string index
                offset += 5 * 0x4; // skip over the 5 words of an attribute
                QString attributeName = ApkManifestFile::uncompressString(binaryData, attributeNameStringIndex, stringTableOffset, stringIndexTableOffset);
                QString attributeValue = attributeValueStringIndex != 0xFFFFFFFF ?
                            ApkManifestFile::uncompressString(binaryData, attributeValueStringIndex, stringTableOffset, stringIndexTableOffset) :
                            "resourceID 0x" + QString::number(attrResId, 16);
                tagAttributes += " " + attributeName + "=\"" + attributeValue + "\"";
            }
            this->t_textData += "<" + tagName + tagAttributes + ">\n";
            continue;
        }
        case APK_MANIFEST_END_TAG: { // XML END TAG
            offset += 6 * 0x4;  // skip APK_MANIFEST_END_TAG data
            QString name = ApkManifestFile::uncompressString(binaryData, nameStringIndex, stringTableOffset, stringIndexTableOffset);
            this->t_textData += "</" + name + ">\n";
            continue;
        }
        case APK_MANIFEST_END_DOC_TAG: // END OF XML DOC TAG
            break;
        default: // unknown tag
            this->t_textData += "  Unrecognized tag code '"+ QString::number(tag, 16) +"' at offset "+ QString::number(offset) + "\n";
  //          prt("  Unrecognized tag code '"+Integer.toHexString(tag0) +"' at offset "+off);
        }
        break;
    }
}


void ApkManifestFile::setCompressedBinaryDevice(QIODevice& binaryDevice) {
    this->setCompressedBinaryData(binaryDevice.readAll()); // read all data from device
}


void ApkManifestFile::setCompressedBinaryFile(const QString& binaryFilePath) {
    QFile binaryFile(binaryFilePath); // file with data
    binaryFile.open(QIODevice::ReadOnly); // open file to read data
    this->setCompressedBinaryDevice(binaryFile); // read data from file
    binaryFile.close(); // close file
}
