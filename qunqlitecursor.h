/*
 * Copyright (c) 2013, galaxyworld.org
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef QUNQLITECURSOR_H
#define QUNQLITECURSOR_H

#include <QObject>

#include "dpointer.h"

extern "C" {
#include "unqlite/unqlite.h"
}

class QUnQLite;
class QUnQLiteCursorPrivate;

class QUnQLiteCursor : public QObject
{
	Q_OBJECT
public:
	enum SeekDirection
	{
		ExactMatch = UNQLITE_CURSOR_MATCH_EXACT,
		Le = UNQLITE_CURSOR_MATCH_LE,
		Ge = UNQLITE_CURSOR_MATCH_GE
	};

	explicit QUnQLiteCursor(QUnQLite *db);
	~QUnQLiteCursor();

	bool reset();

	bool seek(const QString &key, SeekDirection sd);

	bool first();
	bool last();

	bool next();
	bool previous();

	QString key();
	QByteArray value();
	QString valueText();

	bool isValid() const;

	void operator ++();

private:
	D_POINTER

	friend void operator ++(QUnQLiteCursor &cursor);

	QByteArray searchKey;
};

void operator ++(QUnQLiteCursor &cursor);

#endif // QUNQLITECURSOR_H
