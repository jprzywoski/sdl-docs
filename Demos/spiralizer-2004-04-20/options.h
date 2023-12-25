/*
 * options.h
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

#ifndef OPTIONS_H
#define OPTIONS_H

#define HAS_FLAG(x, f)	((x) & (f))
#define SET_FLAG(x, f)	((x) |= (f))
#define DEL_FLAG(x, f)	((x) &= ~(f))

#define F_FSMODE	0x01
#define F_MOBILE	0x02

class Options
{
public:
    Options(int, char *[]);

    int flags;
    const char *pal_img;
    signed short pal_num;
    int reso_x;
    int reso_y;
    int f_delay;
    double start_ang;

private:
    void GetReso(const char *);
};

#endif                                  // OPTIONS_H
