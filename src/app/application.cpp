/****************************************************************************
 **
 ** Copyright (C) 2008-2011 Nokia Corporation and/or its subsidiary(-ies).
 ** Contact: Nokia Corporation (info@qt.nokia.com)
 **
 ** This file is part of the jom project on Trolltech Labs.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 or 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include "application.h"
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <qt_windows.h>
#include <Tlhelp32.h>

namespace NMakeFile {

static bool isSubJOM(const QString &processExeName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;
    bool result = false;
    QHash<DWORD, PROCESSENTRY32> processEntries;
    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(pe);
    if (!Process32First(hSnapshot, &pe)) {
        qWarning("Process32First failed with error code %d.", GetLastError());
        goto done;
    }
    do {
        processEntries.insert(pe.th32ProcessID, pe);
    } while (Process32Next(hSnapshot, &pe));

    const DWORD dwCurrentProcessId = GetCurrentProcessId();
    DWORD dwProcessId = dwCurrentProcessId;
    while (dwProcessId) {
        QHash<DWORD, PROCESSENTRY32>::iterator it = processEntries.find(dwProcessId);
        if (it == processEntries.end())
            break;

        PROCESSENTRY32 &pe = it.value();
        QString exeName = QString::fromUtf16(pe.szExeFile);
        if (pe.th32ProcessID != dwCurrentProcessId && exeName == processExeName) {
            result = true;
            goto done;
        }

        dwProcessId = pe.th32ParentProcessID;
        processEntries.erase(it);
    }

done:
    CloseHandle(hSnapshot);
    return result;
}

Application::Application(int argc, char** argv)
:   QCoreApplication(argc, argv)
{
    QString exeName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    m_bIsSubJOM = NMakeFile::isSubJOM(exeName);
}

} // namespace NMakeFile
