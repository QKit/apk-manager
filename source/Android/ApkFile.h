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

#ifndef _ApkFile_h_
#define _ApkFile_h_

#include "ApkManifestFile.h"

/*!
 * \brief Class with methods to read apk file.
 */
class ApkFile : public QObject {
    Q_OBJECT
public:
    /*!
     * \brief Constructor.
     */
    ApkFile() {}

    /*!
     * \brief Constructor.
     * \param filePath string with the path to the file
     */
    ApkFile(const QString& filePath);

    /*!
     * \brief Get apk manifest file.
     * \return pointer to manifest file object
     */
    const ApkManifestFile* getManifestFile() const {return &(this->manifestFile);}

public slots:
    /*!
     * \brief Set data as compressed binary file data.
     * \param filePath string with the path to the file
     */
    void setPath(const QString& filePath);

private:
    ApkManifestFile manifestFile; // !< apk manifest file
};

#endif // __ApkFile_h_
