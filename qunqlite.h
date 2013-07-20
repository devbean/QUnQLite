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

#ifndef QUNQLITE_H
#define QUNQLITE_H

#include <QObject>

extern "C" {
#include "UnQLite/UnQLite.h"
}

class QUnQLiteCursor;
class QUnQLiteCursorPrivate;
class QUnQLitePrivate;

class QUnQLite : public QObject
{
    Q_OBJECT
public:
    enum OpenMode
    {
        Create           = UNQLITE_OPEN_CREATE,
        ReadWrite        = UNQLITE_OPEN_READWRITE,
        ReadOnly         = UNQLITE_OPEN_READONLY,
        ReadOnlyWithMMap = UNQLITE_OPEN_READONLY | UNQLITE_OPEN_MMAP
    };

    enum ResultCode
    {
        Ok              = UNQLITE_OK,

        NoMemory        = UNQLITE_NOMEM,
        Abort           = UNQLITE_ABORT,
        IoError         = UNQLITE_IOERR,
        CorruptPointer  = UNQLITE_CORRUPT,
        Locked          = UNQLITE_LOCKED,
        Busy            = UNQLITE_BUSY,
        Done            = UNQLITE_DONE,
        PermissionError = UNQLITE_PERM,
        NotImplemented  = UNQLITE_NOTIMPLEMENTED,
        NotFound        = UNQLITE_NOTFOUND,
        NoSuchFunction  = UNQLITE_NOOP,
        Invalid         = UNQLITE_INVALID,
        EndOfInput      = UNQLITE_EOF,
        UnknownError    = UNQLITE_UNKNOWN,
        Limit           = UNQLITE_LIMIT,
        Exists          = UNQLITE_EXISTS,
        Empty           = UNQLITE_EMPTY,
        CompileError    = UNQLITE_COMPILE_ERR,
        VMError         = UNQLITE_VM_ERR,
        Full            = UNQLITE_FULL,
        CannotOpen      = UNQLITE_CANTOPEN,
        IsReadOnly      = UNQLITE_READ_ONLY,
        LockingError    = UNQLITE_LOCKERR
    };

    QUnQLite();
    ~QUnQLite();

    ResultCode lastErrorCode() const;

    bool open(const QString &name, OpenMode mode);
    bool close();

    bool append(const QString &key, const QString &value);
    bool store(const QString &key, const QString &value);

    QByteArray fetch(const QString &key);

    bool remove(const QString &key);

    QUnQLiteCursor * cursor() const;

    bool begin();
    bool commit();
    bool rollback();

private:
    friend class QUnQLiteCursor;
    friend class QUnQLiteCursorPrivate;
    Q_DECLARE_PRIVATE(QUnQLite)
};

#endif // QUNQLITE_H
