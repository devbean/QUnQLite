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

#ifndef DPOINTER_H
#define DPOINTER_H

template <typename T>
class DPointer
{
public:
    DPointer() : d(new T()) {}

    /*
     * We can implement by variadic templates,
     * but some compilers, such as VC11 does not
     * support it yet. So we have to add these ugly
     * override constructors.
     *
     * template <typename T>
     * template <typename ...Args>
     * DPointer<T>::DPointer(Args && ... args)
     *     : d(new T(std::forward<Args>(args)... ))
     * {
     * }
     */

    template <typename Arg1>
    DPointer(Arg1 && arg1) : d(new T(arg1))
    {
    }

    template <typename Arg1, typename Arg2>
    DPointer(Arg1 && arg1, Arg2 && arg2)
        : d(new T(arg1, arg2))
    {
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    DPointer(Arg1 && arg1, Arg2 && arg2, Arg3 && arg3)
        : d(new T(arg1, arg2, arg3))
    {
    }

    ~DPointer() {}

    T * operator->() const
    {
        return d.data();
    }

    T * get() const
    {
        return d.data();
    }

private:
    QScopedPointer<T> d;
}; // end of class GOW::DPointer

#define D_POINTER                   \
    class Private;                  \
    friend class Private;           \
    const DPointer<Private> d; \

#define Q_POINTER(CLASS)            \
    friend class CLASS;             \
    CLASS * const q;

#endif // DPOINTER_H
