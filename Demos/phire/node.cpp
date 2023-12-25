// phire node.cpp - Abstract node class
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include "node.h"

// Static root node
node node::rootnode(0);

// draw() all children
void node::draw()
{
    node *tmp = child;

    while (tmp)
    {
        tmp->draw();
        tmp = tmp->next;
    }
}

// run() all children; delete them if they are dead
void node::run()
{
    node *tmp = child;
    node *del;

    while (tmp)
    {
        tmp->run();
        if (tmp->dead)
        {
            del = tmp;
            tmp = tmp->next;
            delete del;
            continue;
        }
        tmp = tmp->next;
    }
}

// Get a new parent
void node::attach(node *p)
{
    detach();
    par = p;
    next = p->child;
    if (next)
    {
        next->prev = this;
    }
    p->child = this;
}

// Detach from current parent
void node::detach()
{
    if (!par)
    {
        return;
    }
    if (par->child == this)
    {
        par->child = next;
    }
    if (prev)
    {
        prev->next = next;
    }
    if (next)
    {
        next->prev = prev;
    }
    par = next = prev = 0;
}

// Mark this object as dead
void node::die()
{
    dead = -1;
}

// Delete all children
void node::clr()
{
    while (child)
    {
        delete child;
    }
}

// Node constructor
node::node(node *p)
{
    par = next = prev = child = 0;
    if (p)
    {
        attach(p);
    }
    dead = 0;
}

// Node destructor
node::~node()
{
    clr();
    detach();
}
