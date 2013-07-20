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
#include "qunqlite_p.h"
#include "qunqlitecursor.h"

/*!
 * \class QUnQLite
 * \brief UnQLite database handle.
 *
 * Each open UnQLite database is represented by this class.
 * It is useful to think of an unqlite pointer as an object.
 * Once an instance is created, \c open() function should be called
 * in order to do more operations. When all operations are done,
 * \c close() need to be invoked.
 */

/*!
 * \brief Constructs an instance of QUnQLite.
 */
QUnQLite::QUnQLite()
{
}

/*!
 * \brief Destructs the instance.
 */
QUnQLite::~QUnQLite()
{
}

/*!
 * \brief Get the result code of the last operation.
 */
QUnQLite::ResultCode QUnQLite::lastErrorCode() const
{
    Q_D(const QUnQLite);
    return d->resultCode;
}

/*!
 * \brief Open a connection to database named \a name
 * with \a mode as open mode.
 *
 * This function is often the first API call that an application makes
 * and is a prerequisite in order to work with the database library.
 *
 * If \a name is ":mem:" or NULL, then a private, in-memory database
 * is created for the connection.
 * The in-memory database will vanish when the database connection is closed.
 *
 * \note This function does not open the target database file.
 * It merely initialize and prepare the database object handle for later usage.
 *
 * You could get database return code by \c lastErrorCode() .
 *
 * \return True if success.
 */
bool QUnQLite::open(const QString &name, OpenMode mode)
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_open(&d->db, name.toUtf8().constData(), mode));
    return d->isSuccess();
}

/*!
 * \brief Close existing unqlite handle.
 *
 * If this function is invoked while a transaction is open, the transaction is
 * automatically committed unless database is set to be disable auto commit.
 * In which case, the database is rolled back.
 *
 * \return True if the unqlite object is successfully destroyed
 * and all associated resources are deallocated.
 */
bool QUnQLite::close()
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_close(d->db));
    return d->isSuccess();
}

/*!
 * \brief Write a new record \a value with \a key into the database.
 *
 * If the record does not exists, it is created. Otherwise, the new data chunk
 * is appended to the end of the old chunk.
 * You can switch to \c store() for an overwrite operation.
 *
 * \return True if success.
 */
bool QUnQLite::append(const QString &key, const QString &value)
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_kv_append(d->db,
                                       key.toUtf8().constData(), key.size(),
                                       value.toUtf8().constData(), value.size()));
    return d->isSuccess();
}

/*!
 * \brief Write a new record \a value with \a key into the database.
 *
 * If the record does not exists, it is created. Otherwise, it is replaced.
 * That is, the new data overwrite the old data.
 * You can switch to \c append() for an append operation.
 *
 * \return True if success.
 */
bool QUnQLite::store(const QString &key, const QString &value)
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_kv_store(d->db,
                                      key.toUtf8().constData(), key.size(),
                                      value.toUtf8().constData(), value.size()));
    return d->isSuccess();
}

/*!
 * \brief Fetch a record with \a key from the database.
 *
 * \return Record data, empty if no such record or something wrong.
 * You could check \c lastErrorCode() to find out if any error.
 */
QByteArray QUnQLite::fetch(const QString &key)
{
    Q_D(QUnQLite);
    qint64 length;
    d->setResultCode(unqlite_kv_fetch(d->db,
                                      key.toUtf8().constData(), key.size(),
                                      NULL, &length));
    if(d->isSuccess()) {
        QByteArray record(length, 0);
        d->setResultCode(unqlite_kv_fetch(d->db,
                                          key.toUtf8().constData(), key.size(),
                                          record.data(), &length));
        return record;
    }
    return QByteArray();
}

/*!
 * \brief To remove a particular record which key is \a key from the database,
 * you can use this high-level thread-safe routine to perform the deletion.
 *
 * \return True if success.
 */
bool QUnQLite::remove(const QString &key)
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_kv_delete(d->db,
                                       key.toUtf8().constData(), key.size()));
    return d->isSuccess();
}

/*!
 * \brief Create a cursor to this database.
 */
QUnQLiteCursor * QUnQLite::cursor() const
{
    return new QUnQLiteCursor(const_cast<QUnQLite *>(this));
}

/*!
 * \brief Begin a write-transaction on the specified database handle.
 *
 * If a write-transaction has already been opened, this function is a no-op.
 * \note For maximum concurrency, it is preferable to let UnQLite start the transaction
 * for you automatically. An automatic transaction is started each time upper-layers
 * or client code request a store, delete or an append operation.
 * \return True if success.
 */
bool QUnQLite::begin()
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_begin(d->db));
    return d->isSuccess();
}

/*!
 * \brief Commit all changes to the database and release the exclusive lock.
 * In other words, make sure that all changes reaches the disk surface.
 *
 * \note Normally, a call to this routine is not necessary since transactions
 * are committed automatically by the engine when the database is closed via QUnQLite::close()
 * unless the UNQLITE_CONFIG_DISABLE_AUTO_COMMIT option is set.
 * In which case, you should manually call QUnQLite::commit().
 * Otherwise, the database is rolled back.
 *
 * \note For maximum concurrency, it is recommended that you commit your transaction
 * manually as soon as you have no more insertions. Also, for very large insertions (More than 20000),
 * you should call unqlite_commit() periodically to free some memory
 * (A new transaction is started automatically in the next insertion).
 * \return True if success.
 */
bool QUnQLite::commit()
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_commit(d->db));
    return d->isSuccess();
}

/*!
 * \brief If a write transaction is open, then all changes made within the transaction
 * are reverted and the current write-transaction is closed
 * (Dropping all exclusive locks on the target database,
 * deletion of the journal file, etc.). Otherwise this routine is a no-op.
 * \return True if success.
 */
bool QUnQLite::rollback()
{
    Q_D(QUnQLite);
    d->setResultCode(unqlite_rollback(d->db));
    return d->isSuccess();
}

/*!
 * \enum QUnQLite::OpenMode
 * \brief These values are intended for use in the 3rd parameter to
 * the \c open() function.
 */

/*!
 * \var QUnQLite::OpenMode QUnQLite::Create
 * \brief If the database does not exists, it is created.
 * Otherwise, it is opened with read+write privileges.
 *
 * This is the recommended access control flags for most applications.
 */

/*!
 * \var QUnQLite::OpenMode QUnQLite::ReadWrite
 * \brief Open the database with read+write privileges.
 *
 * If the database does not exists, an error code is returned.
 */

/*!
 * \var QUnQLite::OpenMode QUnQLite::ReadOnly
 * \brief Open the database in a read-only mode.
 *
 * That is, you cannot perform a store, append, commit or rollback operations
 * with this control flag.
 */

/*!
 * \var QUnQLite::OpenMode QUnQLite::ReadOnlyWithMMap
 * \brief Obtain a read-only memory view of the whole database.
 *
 * You will get significant performance improvements with this combination
 * but your database is still read-only.
 */

/*!
 * \enum QUnQLite::ResultCode
 * \brief Most of the UnQLite public interfaces return an integer result code
 * from the set shown here in order to indicates success or failure.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Ok
 * \brief Successful result.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::NoMemory
 * \brief Out of memory.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Abort
 * \brief Another thread have released this instance.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::IoError
 * \brief IO error.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::CorruptPointer
 * \brief Corrupt pointer.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Locked
 * \brief Forbidden Operation.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Busy
 * \brief The database file is locked.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Done
 * \brief Operation done.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::PermissionError
 * \brief Permission error.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::NotImplemented
 * \brief Method not implemented by the underlying Key/Value storage engine.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::NotFound
 * \brief No such record.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::NoSuchFunction
 * \brief No such method.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Invalid
 * \brief Invalid parameter.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::EndOfInput
 * \brief End of input.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::UnknownError
 * \brief Unknown configuration option.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Limit
 * \brief Database limit reached.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Exists
 * \brief Records exists.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Empty
 * \brief Empty record.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::CompileError
 * \brief Compilation error.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::VMError
 * \brief Virtual machine error.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::Full
 * \brief Full database (unlikely).
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::CannotOpen
 * \brief Unable to open the database file.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::IsReadOnly
 * \brief Read only Key/Value storage engine.
 */

/*!
 * \var QUnQLite::ResultCode QUnQLite::LockingError
 * \brief Locking protocol error.
 */
