/*
 * userval.c
 *
 * MathMap
 *
 * Copyright (C) 1997-2009 Mark Probst
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifndef OPENSTEP
#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#endif

#include "mathmap.h"
#include "userval.h"
#include "tags.h"
#include "drawable.h"

static userval_info_t*
alloc_and_register_userval (userval_info_t **p, const char *name, int type)
{
    userval_info_t *info = (userval_info_t*)malloc(sizeof(userval_info_t));

    memset(info, 0, sizeof(userval_info_t));

    info->name = strdup(name);
    assert(name != 0);

    info->type = type;
    info->next = 0;

    info->index = 0;
    while (*p != 0)
    {
	p = &(*p)->next;
	++info->index;
    }
    *p = info;

    return info;
}

userval_info_t*
lookup_userval (userval_info_t *infos, const char *name)
{
    userval_info_t *info;

    for (info = infos; info != 0; info = info->next)
	if (strcmp(name, info->name) == 0)
	    return info;

    return 0;
}

userval_info_t*
lookup_matching_userval (userval_info_t *infos, userval_info_t *test_info)
{
    userval_info_t *info = lookup_userval(infos, test_info->name);

    if (info == 0 || info->type != test_info->type)
	return 0;

    switch (info->type)
    {
	case USERVAL_INT_CONST :
	    if (info->v.int_const.min != test_info->v.int_const.min
		|| info->v.int_const.max != test_info->v.int_const.max)
		info = 0;
	    break;

	case USERVAL_FLOAT_CONST :
	    if (info->v.float_const.min != test_info->v.float_const.min
		|| info->v.float_const.max != test_info->v.float_const.max)
		info = 0;
	    break;

	case USERVAL_IMAGE :
	    if (info->v.image.flags != test_info->v.image.flags)
		info = 0;
	    break;
    }

    return info;
}

userval_info_t*
register_int_const (userval_info_t **infos, const char *name, int min, int max, int default_value)
{
    userval_info_t *info;

    assert(default_value >= min && default_value <= max);

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_INT_CONST)
	    return 0;
	if (info->v.int_const.min == min && info->v.int_const.max == max)
	{
	    info->v.int_const.default_value = default_value;
	    return info;
	}
	return 0;
    }
    else
    {
	info = alloc_and_register_userval(infos, name, USERVAL_INT_CONST);
	info->v.int_const.min = min;
	info->v.int_const.max = max;
	info->v.int_const.default_value = default_value;
    }

    return info;
}

userval_info_t*
register_float_const (userval_info_t **infos, const char *name, float min, float max, float default_value)
{
    userval_info_t *info;

    assert(default_value >= min && default_value <= max);

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_FLOAT_CONST)
	    return 0;
	if (info->v.float_const.min == min && info->v.float_const.max == max)
	{
	    info->v.float_const.default_value = default_value;
	    return info;
	}
	return 0;
    }
    else
    {
	info = alloc_and_register_userval(infos, name, USERVAL_FLOAT_CONST);
	info->v.float_const.min = min;
	info->v.float_const.max = max;
	info->v.float_const.default_value = default_value;
    }

    return info;
}

userval_info_t*
register_bool (userval_info_t **infos, const char *name, int default_value)
{
    userval_info_t *info;

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_BOOL_CONST)
	    return 0;
	return info;
    }

    info = alloc_and_register_userval(infos, name, USERVAL_BOOL_CONST);
    info->v.bool_const.default_value = default_value;

    return info;
}

userval_info_t*
register_color (userval_info_t **infos, const char *name)
{
    userval_info_t *info;

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_COLOR)
	    return 0;
	return info;
    }

    info = alloc_and_register_userval(infos, name, USERVAL_COLOR);

    return info;
}

userval_info_t*
register_curve (userval_info_t **infos, const char *name)
{
    userval_info_t *info;

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_CURVE)
	    return 0;
	return info;
    }

    info = alloc_and_register_userval(infos, name, USERVAL_CURVE);

    return info;
}

userval_info_t*
register_gradient (userval_info_t **infos, const char *name)
{
    userval_info_t *info;

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_GRADIENT)
	    return 0;
	return info;
    }

    info = alloc_and_register_userval(infos, name, USERVAL_GRADIENT);

    return info;
}

userval_info_t*
register_image (userval_info_t **infos, const char *name, unsigned int flags)
{
    userval_info_t *info;

    info = lookup_userval(*infos, name);
    if (info != 0)
    {
	if (info->type != USERVAL_IMAGE)
	    return 0;
	if (info->v.image.flags != flags)
	    return 0;
	return info;
    }

    info = alloc_and_register_userval(infos, name, USERVAL_IMAGE);

    info->v.image.flags = flags;

    return info;
}

static void
calc_image_values (userval_info_t *info, userval_t *val)
{
    int width, height;

    g_assert(info->type == USERVAL_IMAGE);
    g_assert(val->v.image->type == IMAGE_DRAWABLE);
    g_assert(val->v.image->v.drawable != NULL);

    width = val->v.image->pixel_width;
    height = val->v.image->pixel_height;

    val->v.image->v.drawable->scale_x = (width - 1) / 2.0;
    val->v.image->v.drawable->scale_y = (height - 1) / 2.0;

    val->v.image->v.drawable->middle_x = 1.0;
    val->v.image->v.drawable->middle_y = 1.0;
}

static curve_t*
get_default_curve (void)
{
    static curve_t curve;
    static float points[USER_CURVE_POINTS];
    static gboolean inited = FALSE;

    if (!inited)
    {
	int i;

	for (i = 0; i < USER_CURVE_POINTS; ++i)
	    points[i] = (float)i / (float)(USER_CURVE_POINTS - 1);

	curve.values = points;

	inited = TRUE;
    }

    return &curve;
}

static curve_t*
copy_curve (curve_t *curve)
{
    curve_t *copy = g_new(curve_t, 1);

    copy->values = g_new(float, USER_CURVE_POINTS);
    memcpy(copy->values, curve->values, sizeof(float) * USER_CURVE_POINTS);

    return copy;
}

static gradient_t*
copy_gradient (gradient_t *gradient)
{
    gradient_t *copy = g_new(gradient_t, 1);

    copy->values = g_new(color_t, USER_GRADIENT_POINTS);
    memcpy(copy->values, gradient->values, sizeof(color_t) * USER_GRADIENT_POINTS);

    return copy;
}

static void
free_curve (curve_t *curve)
{
    g_assert(curve != get_default_curve());

    g_free(curve->values);
    g_free(curve);
}

static void
free_gradient (gradient_t *gradient)
{
    g_assert(gradient != get_default_gradient());

    g_free(gradient->values);
    g_free(gradient);
}

void
set_userval_to_default (userval_t *val, userval_info_t *info, mathmap_invocation_t *invocation)
{
    switch (info->type)
    {
	case USERVAL_INT_CONST :
	    val->v.int_const = info->v.int_const.default_value;
	    break;

	case USERVAL_FLOAT_CONST :
	    val->v.float_const = info->v.float_const.default_value;
	    break;

	case USERVAL_BOOL_CONST :
	    val->v.bool_const = info->v.bool_const.default_value;
	    break;

	case USERVAL_CURVE :
	    val->v.curve = copy_curve(get_default_curve());
	    break;

	case USERVAL_GRADIENT :
	    val->v.gradient = copy_gradient(get_default_gradient());
	    break;

	case USERVAL_COLOR :
#ifndef OPENSTEP
	    val->v.color.button_value.r =
		val->v.color.button_value.g =
		val->v.color.button_value.b = 0.0;
	    val->v.color.button_value.a = 1.0;
#endif

	    val->v.color.value = COLOR_BLACK;
	    break;

	case USERVAL_IMAGE :
	    {
		input_drawable_t *drawable = get_default_input_drawable();

		if (drawable != NULL)
		    assign_image_userval_drawable(info, val,
						  copy_input_drawable(drawable));
		else
		    val->v.image = NULL;
	    }
	    break;
    }
}

void
instantiate_userval (userval_t *val, userval_info_t *info, mathmap_invocation_t *invocation)
{
    set_userval_to_default(val, info, invocation);
}

userval_t*
instantiate_uservals (userval_info_t *infos, mathmap_invocation_t *invocation)
{
    int n;
    userval_info_t *info;
    userval_t *uservals;

    n = 0;
    for (info = infos; info != 0; info = info->next)
	++n;

    uservals = (userval_t*)malloc(n * sizeof(userval_t));
    memset(uservals, 0, n * sizeof(userval_t));

    for (info = infos; info != 0; info = info->next)
	instantiate_userval(&uservals[info->index], info, invocation);

    return uservals;
}

void
free_uservals (userval_t *uservals, userval_info_t *infos)
{
    userval_info_t *info;

    for (info = infos; info != 0; info = info->next)
    {
	switch (info->type)
	{
	    case USERVAL_CURVE :
		free_curve(uservals[info->index].v.curve);
		break;

	    case USERVAL_GRADIENT :
		free_gradient(uservals[info->index].v.gradient);
		break;

	    case USERVAL_IMAGE :
		g_assert(uservals[info->index].v.image != NULL);
		if (uservals[info->index].v.image->type == IMAGE_DRAWABLE) {
		    g_assert(uservals[info->index].v.image->v.drawable);
		    free_input_drawable(uservals[info->index].v.image->v.drawable);
		}
		break;
	}
    }
}

void
free_userval_infos (userval_info_t *infos)
{
    while (infos != 0)
    {
	userval_info_t *next = infos->next;

	free(infos->name);
	free(infos);

	infos = next;
    }
}

void
copy_userval (userval_t *dst, userval_t *src, int type)
{
    switch (type)
    {
	case USERVAL_INT_CONST :
	case USERVAL_FLOAT_CONST :
	case USERVAL_BOOL_CONST :
	case USERVAL_COLOR :
	    dst->v = src->v;
	    break;

	case USERVAL_CURVE :
	    dst->v.curve = copy_curve(src->v.curve);
	    break;

	case USERVAL_GRADIENT :
	    dst->v.gradient = copy_gradient(src->v.gradient);
	    break;

	case USERVAL_IMAGE :
	    {
		input_drawable_t *dst_drawable;

		g_assert (src->v.image != NULL);
		g_assert (src->v.image->type == IMAGE_DRAWABLE);

		if (dst->v.image->type == IMAGE_DRAWABLE)
		    dst_drawable = dst->v.image->v.drawable;
		else
		    dst_drawable = NULL;

		if (src->v.image->v.drawable != 0)
		{
		    input_drawable_t *copy = copy_input_drawable(src->v.image->v.drawable);

		    dst->v.image = &copy->image;
		}
		else
		    dst->v.image = 0;

		if (dst_drawable != 0)
		    free_input_drawable(dst_drawable);
	    }
	    break;

	default :
	    assert(0);
    }
}

void
assign_image_userval_drawable (userval_info_t *info, userval_t *val, input_drawable_t *drawable)
{
    g_assert(info->type == USERVAL_IMAGE);

    if (val->v.image != NULL)
    {
	g_assert(val->v.image->type == IMAGE_DRAWABLE);

	if (val->v.image->v.drawable != NULL)
	    free_input_drawable(val->v.image->v.drawable);
    }

    val->v.image = &drawable->image;

    calc_image_values(info, val);
}

const char*
userval_type_name (int type)
{
    switch (type)
    {
	case USERVAL_INT_CONST : return "int";
	case USERVAL_FLOAT_CONST : return "float";
	case USERVAL_BOOL_CONST : return "bool";
	case USERVAL_COLOR : return "color";
	case USERVAL_CURVE : return "curve";
	case USERVAL_GRADIENT : return "gradient";
	case USERVAL_IMAGE : return "image";
	default : g_assert_not_reached();
    }
}

#ifndef OPENSTEP
static void
userval_int_update (GtkAdjustment *adjustment, userval_t *userval)
{
    userval->v.int_const = (int)lrint(adjustment->value);

    user_value_changed();
}

static void
userval_float_update (GtkAdjustment *adjustment, userval_t *userval)
{
    userval->v.float_const = adjustment->value;

    user_value_changed();
}

static void
userval_bool_update (GtkToggleButton *button, userval_t *userval)
{
    if (gtk_toggle_button_get_active(button))
	userval->v.bool_const = 1.0;
    else
	userval->v.bool_const = 0.0;

    user_value_changed();
}

static void
userval_color_update (GtkWidget *color_well, userval_t *userval)
{
    gimp_color_button_get_color(GIMP_COLOR_BUTTON(color_well), &userval->v.color.button_value);
    userval->v.color.value = MAKE_RGBA_COLOR_FLOAT(userval->v.color.button_value.r,
						   userval->v.color.button_value.g,
						   userval->v.color.button_value.b,
						   userval->v.color.button_value.a);

    user_value_changed();
}

static void
user_image_update (gint32 id, void **user_data)
{
    userval_info_t *info = (userval_info_t*)user_data[0];
    userval_t *userval = (userval_t*)user_data[1];
    GimpDrawable *drawable;

    g_assert(userval->v.image != NULL);
    g_assert(userval->v.image->type == IMAGE_DRAWABLE);

    if (userval->v.image->v.drawable != 0
	&& get_gimp_input_drawable(userval->v.image->v.drawable) == gimp_drawable_get(id))
	return;

    drawable = gimp_drawable_get(id);
    assert(drawable != 0);

    assign_image_userval_drawable(info, userval, alloc_gimp_input_drawable(drawable, FALSE));

    user_value_changed();
}

static int
make_table_entry_for_userval (userval_info_t *info, int *have_input_image)
{
    if (info->type == USERVAL_GRADIENT)
	return 0;
    if (info->type == USERVAL_IMAGE)
    {
	if (*have_input_image)
	    return 1;
	*have_input_image = 1;
	return 0;
    }
    return 1;
}

static float
increment_for_range (float range, int *_exponent)
{
    int exponent, j;
    float increment;

    j = exponent = (int)(floor(log10(range)) - 3);
    increment = 1.0;
    while (j > 0)
    {
	increment *= 10.0;
	--j;
    }
    while (j < 0)
    {
	increment /= 10.0;
	++j;
    }

    if (_exponent != 0)
	*_exponent = exponent;

    return increment;
}

GtkWidget*
make_slider_spin_button_box (GtkObject *adjustment, GtkSignalFunc update_func, userval_t *userval,
			     float increment, int digits)
{
    GtkWidget *hscale, *spin_button, *box;

    gtk_signal_connect(adjustment, "value_changed", update_func, userval);

    hscale = gtk_hscale_new(GTK_ADJUSTMENT(adjustment));
    gtk_scale_set_digits(GTK_SCALE(hscale), digits);
    gtk_scale_set_draw_value(GTK_SCALE(hscale), FALSE);
    gtk_widget_show(hscale);

    spin_button = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), increment, digits);
    gtk_widget_show(spin_button);

    box = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hscale, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), spin_button, FALSE, FALSE, 0);

    return box;
}

static gboolean
has_userval_default (userval_info_t *info)
{
    switch (info->type)
    {
	case USERVAL_INT_CONST :
	case USERVAL_FLOAT_CONST :
	case USERVAL_BOOL_CONST :
	    return TRUE;

	default :
	    return FALSE;
    }
}

static void
default_button_clicked (GtkWidget *widget, userval_t *userval)
{
    userval_info_t *info = g_object_get_data(G_OBJECT(widget), "userval-info");

    switch (info->type)
    {
	case USERVAL_INT_CONST :
	    if (userval->v.int_const == info->v.int_const.default_value)
		return;
	    userval->v.int_const = info->v.int_const.default_value;
	    gtk_adjustment_set_value(GTK_ADJUSTMENT(userval->widget_object), userval->v.int_const);
	    break;

	case USERVAL_FLOAT_CONST :
	    if (userval->v.float_const == info->v.float_const.default_value)
		return;
	    userval->v.float_const = info->v.float_const.default_value;
	    gtk_adjustment_set_value(GTK_ADJUSTMENT(userval->widget_object), userval->v.float_const);
	    break;

	case USERVAL_BOOL_CONST :
	    if ((userval->v.bool_const && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(userval->widget_object))) ||
		(!userval->v.bool_const && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(userval->widget_object))))
		return;
	    userval->v.bool_const = info->v.bool_const.default_value;
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(userval->widget_object), userval->v.bool_const);
	    break;

	default :
	    g_assert_not_reached ();
    }
}

GtkWidget*
make_userval_table (userval_info_t *infos, userval_t *uservals)
{
    int i;
    userval_info_t *info;
    GtkWidget *table;
    int have_input_image;

    have_input_image = 0;
    i = 0;
    for (info = infos; info != 0; info = info->next)
	if (make_table_entry_for_userval(info, &have_input_image))
	    ++i;

    if (i == 0)
	return 0;

    table = gtk_table_new(i, 2, FALSE);

    have_input_image = 0;
    i = 0;
    for (info = infos; info != 0; info = info->next)
    {
	GtkObject *object = NULL;
	GtkWidget *widget = NULL;
	GtkWidget *label, *widget_to_add;
	GtkAttachOptions xoptions = GTK_FILL | GTK_EXPAND, yoptions = 0;
	gboolean have_default;

	if (!make_table_entry_for_userval(info, &have_input_image))
	    continue;

	have_default = has_userval_default (info);

	label = gtk_label_new(info->name);
	gtk_table_attach(GTK_TABLE(table), label, 0, 1, i, i + 1, 0, 0, 0, 0);
	gtk_widget_show(label);

	switch (info->type)
	{
	    case USERVAL_INT_CONST :
		{
		    int range = info->v.int_const.max - info->v.int_const.min;
		    int increment = (int)MAX(increment_for_range(range, NULL), 1.0);

		    object = gtk_adjustment_new(uservals[info->index].v.int_const,
						info->v.int_const.min,
						info->v.int_const.max,
						increment, increment * 10, 0.0);
		    widget = make_slider_spin_button_box(object, (GtkSignalFunc)userval_int_update,
							 &uservals[info->index], increment, 0);
		}
		break;

	    case USERVAL_FLOAT_CONST :
		{
		    int exponent;
		    float range = info->v.float_const.max - info->v.float_const.min;
		    float increment = increment_for_range(range, &exponent);
		    int digits = (exponent < 0) ? -exponent : 0;

		    object = gtk_adjustment_new(uservals[info->index].v.float_const,
						info->v.float_const.min,
						info->v.float_const.max,
						increment, increment * 10, 0.0);
		    widget = make_slider_spin_button_box(object, (GtkSignalFunc)userval_float_update,
							 &uservals[info->index], increment, digits);
		}
		break;

	    case USERVAL_BOOL_CONST :
		widget = gtk_check_button_new();
		object = GTK_OBJECT (widget);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
					     uservals[info->index].v.bool_const != 0.0);
		gtk_signal_connect(GTK_OBJECT(widget), "toggled",
				   (GtkSignalFunc)userval_bool_update,
				   &uservals[info->index]);
		break;

	    case USERVAL_COLOR :
		widget = gimp_color_button_new(info->name, 32, 16, &uservals[info->index].v.color.button_value,
					       GIMP_COLOR_AREA_SMALL_CHECKS);
		object = GTK_OBJECT (widget);
		gtk_signal_connect(GTK_OBJECT(widget), "color_changed",
				   (GtkSignalFunc)userval_color_update,
				   &uservals[info->index]);
		break;

	    case USERVAL_CURVE :
		{
		    gfloat vector[USER_CURVE_POINTS];
		    int j;

		    for (j = 0; j < USER_CURVE_POINTS; ++j)
			vector[j] = uservals[info->index].v.curve->values[j] * (USER_CURVE_POINTS - 1);

		    widget = gtk_gamma_curve_new();
		    object = GTK_OBJECT (widget);
		    gtk_curve_set_range(GTK_CURVE(GTK_GAMMA_CURVE(widget)->curve),
					0, USER_CURVE_POINTS - 1, 0, USER_CURVE_POINTS - 1);
		    gtk_curve_set_vector(GTK_CURVE(GTK_GAMMA_CURVE(widget)->curve),
					 USER_CURVE_POINTS, vector);
		    gtk_curve_set_range(GTK_CURVE(GTK_GAMMA_CURVE(widget)->curve), 0, 1, 0, 1);

		    yoptions = GTK_FILL | GTK_EXPAND;
		}
		break;

	    case USERVAL_IMAGE :
		{
		    GtkWidget *menu;
		    gint32 drawable_id = -1;
		    void **user_data;

		    g_assert(uservals[info->index].v.image != NULL);
		    g_assert(uservals[info->index].v.image->type == IMAGE_DRAWABLE);

		    /* FIXME: extremely ugly hack - memory will never be reclaimed! */
		    user_data = (void**)malloc(sizeof(void*) * 2);
		    assert(user_data != 0);
		    user_data[0] = info;
		    user_data[1] = &uservals[info->index];

		    if (uservals[info->index].v.image->v.drawable != 0)
		    {
			GimpDrawable *drawable = get_gimp_input_drawable(uservals[info->index].v.image->v.drawable);

			if (drawable != 0)
			    drawable_id = GIMP_DRAWABLE_ID(drawable);
		    }

		    widget = gtk_option_menu_new();
		    object = GTK_OBJECT (widget);
		    menu = gimp_drawable_menu_new(NULL, (GimpMenuCallback)user_image_update,
						  user_data, drawable_id);
		    gtk_option_menu_set_menu(GTK_OPTION_MENU(widget), menu);
		}
		break;

	    case USERVAL_GRADIENT :
		g_assert_not_reached ();

	    default :
		g_assert_not_reached ();
	}

	if (widget != NULL && have_default)
	{
	    GtkWidget *default_button;

	    widget_to_add = gtk_hbox_new (FALSE, 2);

	    gtk_box_pack_start(GTK_BOX(widget_to_add), widget, TRUE, TRUE, 0);

	    default_button = gtk_button_new_with_label(_("Default"));
	    g_object_set_data(G_OBJECT(default_button), "userval-info", info);
	    gtk_signal_connect(GTK_OBJECT(default_button), "clicked", (GtkSignalFunc)default_button_clicked, &uservals[info->index]);
	    gtk_box_pack_start(GTK_BOX(widget_to_add), default_button, FALSE, FALSE, 0);
	    gtk_widget_show(default_button);
	}
	else
	    widget_to_add = widget;

	if (widget_to_add != NULL)
	{
	    g_assert (widget != NULL && object != NULL);
	    gtk_table_attach(GTK_TABLE(table), widget_to_add, 1, 2, i, i + 1, xoptions, yoptions, 0, 0);
	    gtk_widget_show(widget);
	    gtk_widget_show(widget_to_add);
	}
	else
	    g_assert (object == NULL);

	uservals[info->index].widget_object = object;

	++i;
    }

    gtk_widget_show(table);

    return table;
}

void
update_uservals (userval_info_t *infos, userval_t *uservals)
{
    userval_info_t *info;

    for (info = infos; info != 0; info = info->next)
	if (uservals[info->index].widget_object != NULL && info->type == USERVAL_CURVE)
	    gtk_curve_get_vector(GTK_CURVE(GTK_GAMMA_CURVE(uservals[info->index].widget_object)->curve),
				 USER_CURVE_POINTS,
				 uservals[info->index].v.curve->values);
}
#endif
