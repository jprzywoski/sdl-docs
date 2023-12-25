// phire main.cpp - Main program functionality
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include "app.h"

// Main function
int main(int argc, char **argv)
{
    int err;

    err = app_cmdl(argc, argv);
    if (err)
    {
        return err - 1;
    }
    err = (app_init() ? 1 : 0);
    if (!err)
    {
        app_run();
    }
    app_done();
    return err;
}
