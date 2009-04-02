/****************************************************************************
 **
 ** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 ** Contact: Qt Software Information (qt-info@nokia.com)
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
#pragma once

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStack>

namespace NMakeFile {

class MacroTable;
class PPExpression;

class Preprocessor
{
public:
    Preprocessor();
    ~Preprocessor();

    void setMacroTable(MacroTable* macroTable);
    MacroTable* macroTable() { return m_macroTable; }
    bool openFile(const QString& filename);
    QString readLine();

    bool isWhiteSpace(const QChar& ch)
    {
        return ch == ' ' || ch == '\t';
    }

    uint lineNumber() const
    {
        return m_lineNumber;
    }

private:
    bool internalOpenFile(QString fileName);
    void basicReadLine(QString& line);
    bool parseMacro(const QString& line);
    bool parsePreprocessingDirective(const QString& line);
    bool isPreprocessingDirective(const QString& line, QString& directive, QString& value);
    void skipUntilNextMatchingConditional();
    int evaluateExpression(const QString& expr);
    void error(const QString& msg);

private:
    struct TextFile
    {
        QFile*  file;
        QTextStream* stream;
        QString oldCurrentDir;

        TextFile()
            : file(0), stream(0)
        {}

        TextFile(const TextFile& rhs)
            : file(rhs.file), stream(rhs.stream), oldCurrentDir(rhs.oldCurrentDir)
        {}

        TextFile& operator=(const TextFile& rhs)
        {
            file = rhs.file;
            stream = rhs.stream;
            oldCurrentDir = rhs.oldCurrentDir;
            return *this;
        }
    };

    QStack<TextFile>    m_fileStack;
    MacroTable*         m_macroTable;
    uint                m_lineNumber;
    QRegExp             m_rexMacro;
    QRegExp             m_rexPreprocessingDirective;
    uint                m_conditionalDepth;
    bool                m_followElseBranch;
    PPExpression*       m_expressionParser;
};

} //namespace NMakeFile