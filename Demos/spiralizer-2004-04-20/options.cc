/*
 * options.cc
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

#include "options.h"

#include <getopt.h>

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>

void print_help();

using namespace std;

Options::Options(int argc, char *argv[]): flags(0), pal_img(NULL),
    pal_num(-1), // tells Screen to randomize palette number
    reso_x(0), reso_y(0), f_delay(0), start_ang(M_PI)
{
    struct option long_options[] =
    {
        { "fullscreen", no_argument, 0, 'f' }, { "mobile", no_argument, 0, 'm' }, { "help", no_argument, 0, 'h' },
        { "resolution", required_argument, 0, 'r' }, { "palettes", required_argument, 0, 'p' }, {
            "palnum",
            required_argument, 0,
            'n'
        }, { "delay",
            required_argument,
            0, 'd'
        },
        { 0, 0, 0, 0 }
    };
    int c, option_index = 0;

    while (1)
    {
        c = getopt_long(argc, argv, "fr:p:n:d:mh", long_options, &option_index);
        if (c == -1)
        {
            break;
        }
        switch (c)
        {
        case 'f':
            SET_FLAG(flags, F_FSMODE);
            break;
        case 'm':
            SET_FLAG(flags, F_MOBILE);
            break;
        case 'h':
            print_help();
            exit(0);
        case 'r':
            GetReso(optarg);
            break;
        case 'p':
            pal_img = optarg;
            break;
        case 'n':
            pal_num = atoi(optarg);
            break;
        case 'd':
            f_delay = atoi(optarg);
            break;

        case '?':
            print_help();
            exit(1);
        }
    }
    if (optind < argc)
    {
        print_help();
        exit(1);
    }
    if (!reso_x && !reso_y)
    {
        reso_x = HAS_FLAG(flags, F_MOBILE) ? 800 : 320;
        reso_y = HAS_FLAG(flags, F_MOBILE) ? 600 : 240;
    }
    if (!pal_img)
    {
        pal_img = HAS_FLAG(flags, F_MOBILE) ? "palettes/soft.bmp" : "palettes/hard.bmp";
    }
}

void Options::GetReso(const char *reso_str)
{
    char *str, *c;

    str = strdup(reso_str);
    if (!str)
    {
        cerr << "Out of memory\n";
        throw;
    }
    c = strchr(str, 'x');
    if (!c)
    {
        cerr << "Resolution must be in \"800x600\" format\n";
        throw;
    }
    *c = 0;
    reso_x = atoi(str);
    reso_y = atoi(c + 1);
    if (reso_x < 0 || reso_y < 0)
    {
        cout << "A resolution of " << reso_x << 'x' << reso_y << "? Hilarious.\n";
        exit(1);
    }
    if (reso_x > 2048 || reso_y > 1536)
    {
        cout << "The resolution can't exceed 2048x1536\n";
        exit(1);
    }
    free(str);
}

void print_help()
{
    static const char *help_text = "Usage: spiralizer [OPTIONS]\n" "\n" "Options:\n"
                                   "  -f, --fullscreen           Run in fullscreen mode.\n"
                                   "  -m, --mobile               Run mobile mode. Looks nice :P\n"
                                   "  -h, --help                 Prints this help text and exits.\n" "\n"
                                   "  -r STR, --resolution=STR   Set resolution to STR.\n"
                                   "                             STR is in \"800x600\" format.\n"
                                   "  -p IMG, --palettes=IMG     Load palettes from IMG(a bitmap image path).\n"
                                   "                             Each row in the image represents a palette, out\n"
                                   "                             of which the 256 left-most pixels are read.\n"
                                   "  -n NUM, --palnum=NUM       Set palette number to NUM.\n"
                                   "                             The default palette image height is 3 pixels so\n"
                                   "                             NUM ranges 0...2 when no bitmap image is defined.\n"
                                   "  -d NUM, --delay=NUM        Sleep NUM milliseconds on every frame.\n";

    cout << help_text << endl;
}
