/*******************************************************************************
*                                                                              *
*  Apk file class implementation.                                              *
*                                                                              *
*  Copyright (C) 2012 Kirill Chuvilin.                                         *
*  Contact: Kirill Chuvilin (kirill.chuvilin@gmail.com, kirill.chuvilin.pro)   *
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

#include "ApkFile.h"

#include "quazip.h"
#include "quazipfile.h"

ApkFile::ApkFile(const QString& filePath) {
    this->setPath(filePath);
}


void ApkFile::setPath(const QString& filePath) {
    QuaZip zipFile(filePath); // apk file as zip archive
    zipFile.open(QuaZip::mdUnzip); // open file to unzip
    zipFile.setCurrentFile("AndroidManifest.xml");
    QuaZipFile zipppedManifestFile(&zipFile); // manifest file in zip archive
    zipppedManifestFile.open(QIODevice::ReadOnly); // open zipped file
    this->manifestFile.setCompressedBinaryDevice(zipppedManifestFile); // read data
    zipppedManifestFile.close(); // close zipped file
    zipFile.close(); // close file
}
