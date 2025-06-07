/* -*- c -*- */

/*
 * loadsave.c
 *
 * MathMap
 *
 * Copyright (C) 2008-2010 Mark Probst
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

#include "designer.h"
#include "../expression_db.h"

static lisp_object_t*
load_lisp (const char *filename, lisp_object_t **node_list)
{
    lisp_stream_t stream;
    lisp_object_t *obj;

    if (lisp_stream_init_path(&stream, filename) == NULL)
	return NULL;
    obj = lisp_read(&stream);
    lisp_stream_free_path(&stream);

    if (!lisp_match_string("(design . #?(list))", obj, node_list))
    {
	lisp_free(obj);
	return NULL;
    }

    return obj;
}

static char*
get_string_property (lisp_object_t *proplist, const char *name)
{
    lisp_object_t *val = lisp_proplist_lookup_symbol(proplist, name);

    if (lisp_nil_p(val))
	return NULL;
    if (!lisp_string_p(val))
	return NULL;

    return g_strdup(lisp_string(val));
}

static void
get_metadata_from_proplist (lisp_object_t *proplist, expression_metadata_t *meta)
{
    lisp_object_t *tags;

    meta->name = get_string_property(proplist, ":name");
    meta->name_space = get_string_property(proplist, ":name-space");
    meta->title = get_string_property(proplist, ":title");

    meta->tags = NULL;
    tags = lisp_proplist_lookup_symbol(proplist, ":tags");

    while (lisp_cons_p(tags))
    {
	lisp_object_t *tag = lisp_car(tags);

	if (lisp_string_p(tag))
	    meta->tags = g_list_prepend(meta->tags, g_strdup(lisp_string(tag)));

	tags = lisp_cdr(tags);
    }
}

gboolean
designer_load_design_metadata (const char *filename, expression_metadata_t *meta)
{
    lisp_object_t *node_list;
    lisp_object_t *obj = load_lisp(filename, &node_list);
    lisp_object_t *proplist;

    if (obj == NULL)
	return FALSE;

    proplist = obj;
    while (lisp_cons_p(proplist))
    {
	if (lisp_symbol_p(lisp_car(proplist)) && lisp_symbol(lisp_car(proplist))[0] == ':')
	    break;
	proplist = lisp_cdr(proplist);
    }

    get_metadata_from_proplist(proplist, meta);

    lisp_free(obj);

    return TRUE;
}

designer_design_t*
designer_load_design (designer_design_type_t *design_type, const char *filename,
		      designer_design_loaded_callback_t loaded_callback,
		      designer_node_aux_load_callback_t node_aux_load,
		      designer_design_aux_load_callback_t design_aux_load,
		      gpointer user_data)
{
    designer_design_t *design;
    lisp_object_t *obj, *node_list, *iter, *design_aux, *root;
    lisp_object_t *design_proplist = lisp_nil();
    expression_metadata_t meta;

    obj = load_lisp(filename, &node_list);
    if (obj == NULL)
	return NULL;

    design = designer_make_design(design_type, "__untitled_design__");
    g_assert (design != NULL);

    /* Add all the nodes first */
    iter = node_list;
    while (!lisp_nil_p(iter))
    {
	lisp_object_t *node_proplist;

	g_assert(lisp_cons_p(iter));

	if (lisp_match_string("(node . #?(list))", lisp_car(iter), &node_proplist))
	{
	    lisp_object_t *name = lisp_proplist_lookup_symbol(node_proplist, ":name");
	    lisp_object_t *type = lisp_proplist_lookup_symbol(node_proplist, ":type");
	    designer_node_t *node;

	    g_assert(lisp_string_p(name));
	    g_assert(lisp_string_p(type));

	    node = designer_add_node(design, lisp_string(name), lisp_string(type));
	    if (node == NULL)
	    {
		designer_free_design(design);
		return NULL;
	    }

#ifdef DEBUG_OUTPUT
	    printf("added node %s of type %s\n", lisp_string(name), lisp_string(type));
#endif
	}
	else
	{
	    design_proplist = iter;
	    break;
	}

	iter = lisp_cdr(iter);
    }

    /* Now connect the slots */
    iter = node_list;
    while (!lisp_nil_p(iter))
    {
	lisp_object_t *node_proplist;

	g_assert(lisp_cons_p(iter));

	if (lisp_match_string("(node . #?(list))", lisp_car(iter), &node_proplist))
	{
	    lisp_object_t *name = lisp_proplist_lookup_symbol(node_proplist, ":name");
	    lisp_object_t *input_slots = lisp_proplist_lookup_symbol(node_proplist, ":input-slots");
	    designer_node_t *node;

	    g_assert(lisp_string_p(name));
	    g_assert(lisp_nil_p(input_slots) || lisp_cons_p(input_slots));

	    node = designer_get_node_by_name(design, lisp_string(name));
	    g_assert(node != NULL);

	    while (!lisp_nil_p(input_slots))
	    {
		lisp_object_t *vars[3];

		g_assert(lisp_cons_p(input_slots));

		if (lisp_match_string("(#?(string) #?(string) #?(string))", lisp_car(input_slots), vars))
		{
		    designer_node_t *source_node;

		    g_assert(lisp_string_p(vars[0]));
		    g_assert(lisp_string_p(vars[1]));
		    g_assert(lisp_string_p(vars[2]));

		    source_node = designer_get_node_by_name(design, lisp_string(vars[1]));

		    if (source_node == NULL
			|| !designer_connect_nodes_by_slot_name(source_node, lisp_string(vars[2]),
								node, lisp_string(vars[0])))
		    {
			designer_free_design(design);
			return NULL;
		    }
		}

		input_slots = lisp_cdr(input_slots);
	    }
	}
	else
	    break;

	iter = lisp_cdr(iter);
    }

    root = lisp_proplist_lookup_symbol(design_proplist, ":root");
    if (!lisp_nil_p(root))
    {
	designer_node_t *node;

	g_assert(lisp_string_p(root));

	node = designer_get_node_by_name(design, lisp_string(root));
	g_assert(node != NULL);

	design->root = node;
    }

    /* The design is loaded now */
    if (loaded_callback != NULL)
	loaded_callback(design, user_data);

    /* Go through all the nodes and call the aux callbacks */
    iter = node_list;
    while (!lisp_nil_p(iter))
    {
	lisp_object_t *node_proplist;

	g_assert(lisp_cons_p(iter));

	if (lisp_match_string("(node . #?(list))", lisp_car(iter), &node_proplist))
	{
	    lisp_object_t *name = lisp_proplist_lookup_symbol(node_proplist, ":name");
	    lisp_object_t *aux = lisp_proplist_lookup_symbol(node_proplist, ":aux");
	    designer_node_t *node;

	    g_assert(lisp_string_p(name));

	    node = designer_get_node_by_name(design, lisp_string(name));
	    g_assert(node != NULL);

	    if (!lisp_nil_p(aux) && node_aux_load != NULL)
		node_aux_load(node, aux, user_data);
	}
	else
	    break;

	iter = lisp_cdr(iter);
    }

    get_metadata_from_proplist(design_proplist, &meta);
    if (meta.name)
	designer_set_design_name(design, meta.name);

    free_expression_metadata_members(&meta);

    design_aux = lisp_proplist_lookup_symbol(design_proplist, ":aux");
    if (!lisp_nil_p(design_aux) && design_aux_load != NULL)
	design_aux_load(design, design_aux, user_data);

    lisp_free(obj);

    return design;
}

gboolean
designer_save_design (designer_design_t *design, const char *filename,
		      designer_node_aux_print_func_t node_aux_print,
		      designer_design_aux_print_func_t design_aux_print,
		      gpointer user_data)
{
    FILE *out = fopen(filename, "w");
    GSList *list;

    if (out == NULL)
	return FALSE;

    lisp_print_open_paren(out);
    lisp_print_symbol("design", out);

    for (list = design->nodes; list != NULL; list = list->next)
    {
	designer_node_t *node = list->data;
	GSList *slot_list;

	lisp_print_open_paren(out);
	lisp_print_symbol("node", out);

	lisp_print_symbol(":name", out);
	lisp_print_string(node->name, out);

	lisp_print_symbol(":type", out);
	lisp_print_string(node->type->name, out);

	lisp_print_symbol(":input-slots", out);
	lisp_print_open_paren(out);
	for (slot_list = node->input_slots; slot_list != NULL; slot_list = slot_list->next)
	{
	    designer_slot_t *slot = slot_list->data;

	    lisp_print_open_paren(out);
	    lisp_print_string(slot->input_slot_spec->name, out);
	    lisp_print_string(slot->source->name, out);
	    lisp_print_string(slot->output_slot_spec->name, out);
	    lisp_print_close_paren(out);
	}
	lisp_print_close_paren(out);

	if (node_aux_print != NULL)
	{
	    lisp_print_symbol(":aux", out);
	    node_aux_print(node, user_data, out);
	}

	lisp_print_close_paren(out);

	fputc('\n', out);
    }

    lisp_print_symbol(":name", out);
    lisp_print_string(design->name, out);

    if (design->root != NULL)
    {
	lisp_print_symbol(":root", out);
	lisp_print_string(design->root->name, out);
    }

    if (design_aux_print != NULL)
    {
	lisp_print_symbol(":aux", out);
	design_aux_print(design, user_data, out);
    }

    lisp_print_close_paren(out);
    fclose(out);

    return TRUE;
}
