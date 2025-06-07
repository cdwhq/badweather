/* -*- c -*- */

/*
 * internals.h
 *
 * MathMap
 *
 * Copyright (C) 1997-2007 Mark Probst
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __INTERNALS_H__
#define __INTERNALS_H__

#include "tuples.h"

#define MAX_INTERNAL_LENGTH    63

#define CONST_NONE              0
#define CONST_X                 1
#define CONST_Y                 2
#define CONST_T                 4

typedef struct _internal_t
{
    char name[MAX_INTERNAL_LENGTH + 1];
    int index;

    int const_type;

    unsigned int is_used;

    struct _internal_t *next;
} internal_t;

internal_t* register_internal (internal_t **internals, const char *name, int const_type);
internal_t* lookup_internal (internal_t *internals, const char *name, int dont_touch);

void free_internals (internal_t *internals);

int number_of_internals (internal_t *internals);

#endif
