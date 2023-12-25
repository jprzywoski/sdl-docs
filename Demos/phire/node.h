// phire node.h - Abstract node class
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#ifndef _phire_node_h
#define _phire_node_h

#define root (node::rootnode)

class node
{
protected:
    node *par;                          // Parent
    node *prev;                         // Previous node in chain
    node *next;                         // Next node in chain
    node *child;                        // First child node
    int dead;                           // Remove this object?

public:
    static node rootnode;

    node(node *p);

    virtual ~node();

    virtual void draw();
    virtual void run();

    void attach(node *p);
    void detach();
    void die();
    void clr();
};

#endif
