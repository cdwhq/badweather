/*
 * compiler-internals.h
 *
 * MathMap
 *
 * Copyright (C) 2002-2009 Mark Probst
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

#ifndef __COMPILER_INTERNALS_H__
#define __COMPILER_INTERNALS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mathmap.h"
#include "vars.h"
#include "compiler.h"
#include "bitvector.h"

#include "opdefs.h"

#define CLOSURE_ARG(x)			((long)(x))
#define CLOSURE_GET(n,t)		(t)(((long*)info)[(n)])
#define CLOSURE_VAR(t,name,n)		t name = CLOSURE_GET((n),t)

struct _value_t;

typedef struct
{
    int number;
    int last_index;
} temporary_t;

#define MAX_PROMOTABLE_TYPE  TYPE_COMPLEX

#define CONST_MAX            (CONST_Y | CONST_X | CONST_T)
#define CONST_IGNORE	     0x1000

typedef int type_t;

typedef struct _compvar_t
{
    int index;
    variable_t *var;		/* 0 if compvar is a temporary */
    temporary_t *temp;		/* 0 if compvar is a variable */
    int n;			/* n/a if compvar is a temporary */
    type_t type;
    struct _value_t *current;
    struct _value_t *values;
} compvar_t;

struct _statement_list_t;
struct _statement_t;

typedef struct _value_t
{
    compvar_t *compvar;
    int global_index;
    int index;			/* SSA index */
    struct _statement_t *def;
    struct _statement_list_t *uses;
    unsigned int const_type : 3; /* defined in internals.h */
    unsigned int least_const_type_directly_used_in : 3;
    unsigned int least_const_type_multiply_used_in : 3;
    unsigned int have_defined : 1; /* used in c code output */
    struct _value_t *next;	/* next value for same compvar */
} value_t;

/* is_rhs_const_primary() assumes that PRIMARY_VALUE is the only non-const
 * primary type.  */
#define PRIMARY_VALUE          1
#define PRIMARY_CONST	       2

typedef struct
{
    int kind;
    int const_type;		/* different meanings for PRIMARY_VALUE vs PRIMARY_CONST! */
    union
    {
	value_t *value;
	runtime_value_t constant;
    } v;
} primary_t;

#define MAKE_CONST_PRIMARY(name, c_type, type_name)	\
	runtime_value_t \
	make_ ## name ## _runtime_value (c_type name ## _value) \
	{ \
	    runtime_value_t value; \
	    value.name ## _value = name ## _value; \
	    return value; \
	} \
	primary_t \
	make_ ## name ## _const_primary (c_type name ## _const) \
	{ \
	    primary_t primary; \
	    primary.kind = PRIMARY_CONST; \
	    primary.const_type = type_name; \
	    primary.v.constant.name ## _value = name ## _const; \
	    return primary; \
	}

#define TYPE_PROP_CONST      1
#define TYPE_PROP_MAX        2
#define TYPE_PROP_MAX_FLOAT  3

typedef int type_prop_t;

typedef struct _operation_t
{
    int index;
    char *name;
    int num_args;
    type_prop_t type_prop;
    int is_pure;
    int is_foldable;
    type_t const_type;		/* used only if type_prop == TYPE_PROP_CONST */
    type_t arg_types[MAX_OP_ARGS]; /* used only if type_prop == TYPE_PROP_CONST */
} operation_t;

typedef struct _inlining_history_t
{
    filter_t *filter;
    struct _inlining_history_t *next;
} inlining_history_t;

#define RHS_PRIMARY          1
#define RHS_INTERNAL         2
#define RHS_OP               3
#define RHS_FILTER	     4
#define RHS_CLOSURE	     5
#define RHS_TUPLE	     6
#define RHS_TREE_VECTOR      7

typedef struct
{
    int kind;
    union
    {
	primary_t primary;
	internal_t *internal;
	struct
	{
	    operation_t *op;
	    primary_t args[MAX_OP_ARGS];
	} op;
	struct
	{
	    filter_t *filter;
	    primary_t *args;
	    inlining_history_t *history;
	} filter;
	struct
	{
	    filter_t *filter;
	    primary_t *args;
	    inlining_history_t *history;
	} closure;
	struct
	{
	    int length;
	    primary_t *args;
	} tuple;		/* also for tree vectors */
    } v;
} rhs_t;

#define STMT_NIL             0
#define STMT_ASSIGN          1
#define STMT_PHI_ASSIGN      2
#define STMT_IF_COND         3
#define STMT_WHILE_LOOP      4

#define SLICE_XY_CONST       1
#define SLICE_X_CONST        2
#define SLICE_Y_CONST        4
#define SLICE_NO_CONST       8
#define SLICE_IGNORE	     0x1000

typedef struct _statement_t
{
    int kind;
    union
    {
	struct
	{
	    value_t *lhs;
	    rhs_t *rhs;
	    rhs_t *rhs2;	/* only valid for STMT_PHI_ASSIGN */
	    value_t *old_value;	/* only valid for STMT_PHI_ASSIGN */
	} assign;
	struct
	{
	    rhs_t *condition;
	    struct _statement_t *consequent;
	    struct _statement_t *alternative;
	    struct _statement_t *exit;
	} if_cond;
	struct
	{
	    struct _statement_t *entry;
	    rhs_t *invariant;
	    struct _statement_t *body;
	} while_loop;
    } v;
    struct _statement_t *parent;
    unsigned int slice_flags;
    struct _statement_t *next;
} statement_t;

typedef struct _statement_list_t
{
    statement_t *stmt;
    struct _statement_list_t *next;
} statement_list_t;

typedef struct _filter_code_t
{
    filter_t *filter;
    statement_t *first_stmt;
} filter_code_t;

typedef struct
{
    int userval_type;
    int var_type;
    int num_vars;
    int getter_op;
} userval_representation_t;

#define BINDING_USERVAL		1
#define BINDING_INTERNAL	2

typedef struct _binding_values_t
{
    int kind;
    gpointer key;
    struct _binding_values_t *next;
    value_t *values[];
} binding_values_t;

typedef bit_vector_t value_set_t;

extern int compiler_op_index (operation_t *op);

extern compvar_t* make_temporary (type_t type);
#define compiler_make_temporary make_temporary
extern statement_t* make_assign (value_t *lhs, rhs_t *rhs);
#define compiler_make_assign make_assign
extern value_t* make_lhs (compvar_t *compvar);
#define compiler_make_lhs make_lhs
extern rhs_t* make_op_rhs (int op_index, ...);
#define compiler_make_op_rhs make_op_rhs
extern primary_t make_compvar_primary (compvar_t *compvar);
#define compiler_make_compvar_primary make_compvar_primary
extern rhs_t* make_primary_rhs (primary_t primary);
#define compiler_make_primary_rhs make_primary_rhs
extern rhs_t* make_value_rhs (value_t *val);
#define compiler_make_value_rhs make_value_rhs

extern void compiler_reset_have_defined (statement_t *stmt);

extern gboolean compiler_is_permanent_const_value (value_t *value);
extern gboolean compiler_is_temporary_const_value (value_t *value);
extern gboolean compiler_is_const_type_within (int const_type, int lower_bound, int upper_bound);
extern gboolean compiler_is_value_needed_for_const (value_t *value, int const_type);

extern char* compiler_get_value_name (value_t *val);
extern void compiler_print_value (value_t *val);
extern void compiler_print_assign_statement (statement_t *stmt);

extern int compiler_num_filter_args (filter_t *filter);

extern char* compiler_function_name_for_op_rhs (rhs_t *rhs, type_t *promotion_type);

extern statement_t** compiler_emit_stmt_before (statement_t *stmt, statement_t **loc, statement_t *parent);

extern gboolean compiler_rhs_is_pure (rhs_t *rhs);

extern gboolean compiler_stmt_is_assign_with_rhs (statement_t *stmt, int rhs_kind);
extern gboolean compiler_stmt_is_assign_with_op (statement_t *stmt, int op_index);
extern primary_t compiler_stmt_op_assign_arg (statement_t *stmt, int arg_index);

extern void compiler_remove_uses_in_rhs (rhs_t *rhs, statement_t *stmt);

extern void compiler_replace_rhs (rhs_t **rhs, rhs_t *replacement, statement_t *stmt);
extern void compiler_replace_op_rhs_arg (statement_t *stmt, int arg_num, primary_t replacement);

extern value_set_t* compiler_new_value_set (void);
extern void compiler_value_set_add (value_set_t *set, value_t *val);
extern void compiler_value_set_add_set (value_set_t *set, value_set_t *addee);
extern gboolean compiler_value_set_contains (value_set_t *set, value_t *val);
extern value_set_t* compiler_value_set_copy (value_set_t *set);
extern void compiler_free_value_set (value_set_t *set);

extern statement_t* compiler_stmt_unlink (statement_t **stmtp);
extern statement_t** compiler_stmt_insert_before (statement_t *stmt, statement_t **insertion_point);

extern void compiler_for_each_value_in_rhs (rhs_t *rhs, void (*func) (value_t *value, void *info),
					    void *info);
extern void compiler_for_each_value_in_statements (statement_t *stmt,
						   void (*func) (value_t *value, statement_t *stmt, void *info),
						   void *info);
extern void compiler_for_each_value_in_statement (statement_t *stmt,
						  void (*func) (value_t *value, statement_t *stmt, void *info),
						  void *info);

extern int compiler_slice_code (statement_t *stmt, unsigned int slice_flag,
				int (*predicate) (statement_t *stmt, void *info), void *info);

extern filter_code_t* compiler_generate_ir_code (filter_t *filter, int constant_analysis,
						 int convert_types, int timeout, gboolean debug_output);

extern filter_code_t** compiler_compile_filters (mathmap_t *mathmap, int timeout);

extern void compiler_free_pools (mathmap_t *mathmap);

extern unsigned int compiler_slice_flag_for_const_type (int const_type);
extern void compiler_slice_code_for_const (statement_t *stmt, int const_type);

extern gboolean compiler_opt_remove_dead_assignments (statement_t *first_stmt);
extern gboolean compiler_opt_orig_val_resize (statement_t **first_stmt);
extern gboolean compiler_opt_strip_resize (statement_t **first_stmt);
extern gboolean compiler_opt_loop_invariant_code_motion (statement_t **first_stmt);

#define COMPILER_FOR_EACH_VALUE_IN_RHS(rhs,func,...) do { long __clos[] = { __VA_ARGS__ }; compiler_for_each_value_in_rhs((rhs),(func),__clos); } while (0)
#define COMPILER_FOR_EACH_VALUE_IN_STATEMENTS(stmt,func,...) do { long __clos[] = { __VA_ARGS__ }; compiler_for_each_value_in_statements((stmt),(func),__clos); } while (0)
#define COMPILER_FOR_EACH_VALUE_IN_STATEMENT(stmt,func,...) do { long __clos[] = { __VA_ARGS__ }; compiler_for_each_value_in_statement((stmt),(func),__clos); } while (0)
#define COMPILER_SLICE_CODE(stmt,flag,func,...) do { long __clos[] = { __VA_ARGS__ }; compiler_slice_code((stmt),(flag),(func),__clos); } while (0)

#ifdef __cplusplus
}
#endif

#endif
