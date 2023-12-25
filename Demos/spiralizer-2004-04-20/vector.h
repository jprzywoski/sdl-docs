/*
 * vector.h
 *
 * Copyright (C) 2004 Henrik Ala-Uotila
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef VECTOR_H
#define VECTOR_H

struct Vector
{
    Vector(): x(0), y(0) {}
    Vector(const Vector &v): x(v.x), y(v.y) {}
    Vector(const int ax, const int ay): x(ax), y(ay) {}

    inline Vector operator +(const Vector &v) const
    {
        return Vector(x + v.x, y + v.y);
    }
    inline Vector operator -(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y);
    }

    inline Vector operator +=(const Vector &v)
    {
        return *this = (*this + v);
    }
    inline Vector operator -=(const Vector &v)
    {
        return *this = (*this - v);
    }

    inline Vector operator =(const Vector &v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    inline bool operator ==(const Vector &v) const
    {
        return x == v.x && y == v.y;
    }

    int x, y;
};

#endif                                  // VECTOR_H
