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

#include "qunqlite.h"
#include "qunqlitecursor.h"
#include "qunqlite.cpp"

class QUnQLiteCursor::Private
{
public:
    Private(QUnQLiteCursor * q_ptr) : q(q_ptr) {}

    void setResultCode(int rc)
    {
        q_unqlite->d->setResultCode(rc);
    }

    bool isSuccess() const
    {
        return q_unqlite->d->isSuccess();
    }

    QUnQLite *q_unqlite;
    unqlite *db;
    unqlite_kv_cursor *cursor;

    Q_POINTER(QUnQLiteCursor)
};

/*!
 * \class QUnQLiteCursor
 * \brief Cursors provide a mechanism by which you can iterate over
 * the records in a database. Using cursors, you can seek, fetch,
 * move, and delete database records.
 *
 * Cursors can be created by database instances.
 */

/*!
 * \brief Constructs an instance of QUnQLiteCursor.
 *
 * This function is rarely called, use \c QUnQLite::cursor() instead.
 */
QUnQLiteCursor::QUnQLiteCursor(QUnQLite *db) :
    d(this)
{
    d->q_unqlite = db;
    d->db = db->d->db;
    d->setResultCode(unqlite_kv_cursor_init(d->db, &d->cursor));
}

/*!
 * \brief Destructs the instance.
 */
QUnQLiteCursor::~QUnQLiteCursor()
{
    d->setResultCode(unqlite_kv_cursor_release(d->db, d->cursor));
}

/*!
 * \brief Reset current cursor.
 * \return True if success.
 */
bool QUnQLiteCursor::reset()
{
    d->setResultCode(unqlite_kv_cursor_reset(d->cursor));
    return d->isSuccess();
}

/*!
 * \brief Position cursor to given \a key.
 *
 * The last argument is seek direction.
 *
 * \sa QUnQLiteCursor::SeekDirection
 * \note QUnQLiteCursor::Le and QUnQLiteCursor::Ge have sense only if
 * the underlying key/value storage subsystem support range search
 * (i.e: B+Tree, R+Tree, etc.). Otherwise this option is ignored
 * and an exact match is performed.
 * \return True if success.
 */
bool QUnQLiteCursor::seek(const QString &key, QUnQLiteCursor::SeekDirection sd)
{
    d->setResultCode(unqlite_kv_cursor_seek(d->cursor,
                                            key.toUtf8().constData(), key.size(),
                                            sd));
    return d->isSuccess();
}

/*!
 * \brief Seek cursor to first.
 * \return True if success.
 */
bool QUnQLiteCursor::first()
{
    d->setResultCode(unqlite_kv_cursor_first_entry(d->cursor));
    return d->isSuccess();
}

/*!
 * \brief Seek cursor to last.
 * \return True if success.
 */
bool QUnQLiteCursor::last()
{
    d->setResultCode(unqlite_kv_cursor_last_entry(d->cursor));
    return d->isSuccess();
}

/*!
 * \brief Seek cursor to the next item.
 * \return True if success.
 */
bool QUnQLiteCursor::next()
{
    d->setResultCode(unqlite_kv_cursor_next_entry(d->cursor));
    return d->isSuccess();
}

/*!
 * \brief Seek cursor to the previous item.
 * \return True if success.
 */
bool QUnQLiteCursor::previous()
{
    d->setResultCode(unqlite_kv_cursor_prev_entry(d->cursor));
    return d->isSuccess();
}

/*!
 * \brief Get key of current cursor.
 * \return Record key, empty if no such record or something wrong.
 * You could check \c lastErrorCode() to find out if any error.
 */
QByteArray QUnQLiteCursor::key()
{
    int length;
    d->setResultCode(unqlite_kv_cursor_key(d->cursor,
                                           NULL, &length));
    if(d->isSuccess()) {
        QByteArray record(length, 0);
        d->setResultCode(unqlite_kv_cursor_key(d->cursor,
                                               record.data(), &length));
        return record;
    }
    return QByteArray();
}

/*!
 * \brief Get value of current cursor.
 * \return Record data, empty if no such record or something wrong.
 * You could check \c lastErrorCode() to find out if any error.
 */
QByteArray QUnQLiteCursor::value()
{
    qint64 length;
    d->setResultCode(unqlite_kv_cursor_data(d->cursor,
                                            NULL, &length));
    if(d->isSuccess()) {
        QByteArray record(length, 0);
        d->setResultCode(unqlite_kv_cursor_data(d->cursor,
                                                record.data(), &length));
        return record;
    }
    return QByteArray();
}

/*!
 * \brief Check if current cursor is valid.
 */
bool QUnQLiteCursor::isValid() const
{
    return unqlite_kv_cursor_valid_entry(d->cursor) == 1;
}

void QUnQLiteCursor::operator ++()
{
}

/*!
 * \enum QUnQLiteCursor::SeekDirection
 * \brief Seek Direction.
 */

/*!
 * \var QUnQLiteCursor::SeekDirection QUnQLiteCursor::ExactMatch
 * \brief Default search method supported by all key/value storage subsystem.
 *
 * An exact match is performed. If the record exists, the cursor is left pointing to it.
 * Otherwise it is left pointing to EOF and result code is set to QUnQLite::NotFound.
 */

/*!
 * \var QUnQLiteCursor::SeekDirection QUnQLiteCursor::Le
 * \brief The cursor is left pointing to the largest key in the database
 * that is smaller than given key.
 *
 * If the database contains no keys smaller than given key, the cursor is left at EOF.
 */

/*!
 * \var QUnQLiteCursor::SeekDirection QUnQLiteCursor::Ge
 * \brief The cursor is left pointing to the smallest key in the database
 * that is larger than given key.
 *
 * If the database contains no keys greater than given key, the cursor is left at EOF.
 */


void operator ++(QUnQLiteCursor &cursor)
{
    cursor.next();
}
