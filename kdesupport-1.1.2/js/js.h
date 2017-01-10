/*
 * Public API for the JavaScript interpreter.
 * Copyright (c) 1998 New Generation Software (NGS) Oy
 *
 * Author: Markku Rossi <mtr@ngs.fi>
 */

/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 */

/*
 * $Source: /home/kde/kdesupport/js/js.h,v $
 * $Id: js.h,v 1.1.1.1 1998/08/09 21:05:34 kulow Exp $
 */

#ifndef JS_H
#define JS_H

#include <stdio.h>

/*
 * Types and definitions.
 */

/* Byte-code instruction dispatch methods. */
typedef enum
{
  JS_VM_DISPATCH_SWITCH_BASIC,
  JS_VM_DISPATCH_SWITCH,
  JS_VM_DISPATCH_JUMPS
} JSVMDispatchMethod;

typedef struct js_interp_st *JSInterpPtr;

struct js_interp_options_st
{
  unsigned int stack_size;
  JSVMDispatchMethod dispatch_method;
  unsigned int verbose;

  unsigned int no_compiler : 1;
  unsigned int stacktrace_on_error : 1;

  unsigned int annotate_assembler : 1;
  unsigned int debug_info : 1;

  /* Warning flags. */
  unsigned int warn_unused_argument : 1;
  unsigned int warn_unused_variable : 1;
  unsigned int warn_undef : 1;	/* runtime option */
  unsigned int warn_shadow : 1;
  unsigned int warn_with_clobber : 1;

  /* Optimization flags. */
  unsigned int optimize_peephole : 1;
  unsigned int optimize_jumps_to_jumps : 1;
  unsigned int optimize_bc_size : 1;
  unsigned int optimize_heavy : 1;

  /* The standard system streams. */
  FILE *s_stdin;
  FILE *s_stdout;
  FILE *s_stderr;
};

typedef struct js_interp_options_st JSInterpOptions;


/* JavaScript public types. */

#define JS_TYPE_UNDEFINED	0
#define JS_TYPE_NULL		1
#define JS_TYPE_BOOLEAN		2
#define JS_TYPE_INTEGER		3
#define JS_TYPE_STRING		4
#define JS_TYPE_DOUBLE		5
#define JS_TYPE_ARRAY		6

/* String type. */
struct js_type_string_st
{
  unsigned int flags;
  unsigned char *data;
  unsigned int len;
};

typedef struct js_type_string_st JSTypeString;

/* Array type. */
struct js_type_array_st
{
  unsigned int length;
  struct js_type_st *data;
};

typedef struct js_type_array_st JSTypeArray;

/* The type closure. */
struct js_type_st
{
  int type;			/* One of the JS_TYPE_* values. */

  union
  {
    long i;			/* integer or boolean */
    JSTypeString *s;		/* string */
    double d;			/* double */
    JSTypeArray *array;		/* array */

    struct			/* Fillter to assure that this the type */
    {				/* structure has correct size. */
      unsigned long a1;
      unsigned long a2;
    } align;
  } u;
};

typedef struct js_type_st JSType;

/* Function type to free client data. */
typedef void (*JSFreeProc) (void *context);

/* Return types for methods. */
typedef enum
{
  JS_OK,
  JS_ERROR
} JSMethodResult;

/* Function type for global methods. */
typedef JSMethodResult (*JSGlobalMethodProc) (void *context,
					      JSInterpPtr interp, int argc,
					      JSType *argv,
					      JSType *result_return,
					      char *error_return);

/* Function type for JS modules. */
typedef void (*JSModuleInitProc) (JSInterpPtr interp);

/*
 * Prototypes global functions.
 */

/* Initialize interpreter options to system's default values. */
void js_init_default_options (JSInterpOptions *options);

/* Create a new JavaScript interpreter. */
JSInterpPtr js_create_interp (JSInterpOptions *options);

/* Destroy interpreter <interp>. */
void js_destroy_interp (JSInterpPtr interp);

/* Return error message from the latest error. */
const char *js_error_message (JSInterpPtr interp);

/* Evalutate JavaScript code <code> with interpreter <interp>. */
int js_eval (JSInterpPtr interp, char *code);

/* Evaluate JavaScript code <data, datalen> with interpreter <interp>. */
int js_eval_data (JSInterpPtr interp, char *data, unsigned int datalen);

/*
 * Evaluate file <filename> with interpreter <interp>.  File
 * <filename> can contain JavaScript of byte-code.  The function
 * return 1 if the evaluation was successful or 0 otherwise.  If the
 * evaluation failed, the error message can be retrieved with the
 * function js_error_message().
 */
int js_eval_file (JSInterpPtr interp, char *filename);

/* Eval JavaScript file <filename> with interpreter <interp>. */
int js_eval_javascript_file (JSInterpPtr interp, char *filename);

/* Execute a byte-code file <filename> with interpreter <interp>. */
int js_execute_byte_code_file (JSInterpPtr interp, char *filename);

/* Compile JavaScript file <input_file> to assembler or byte-code. */
int js_compile (JSInterpPtr interp, char *input_file, char *assembler_file,
		char *byte_code_file);


/* Type handling. */

/*
 * Create a new string type from <data> that has <length> bytes of data.
 * All fields of <type> belong to the interpreter.
 */
void js_type_make_string (JSInterpPtr interp, JSType *type,
			  unsigned char *data, unsigned int length);

/* Create a new array with capacity of <length> items. */
void js_type_make_array (JSInterpPtr interp, JSType *type,
			 unsigned int length);

/* Set the value of global variable <name> to <value>. */
void js_set_var (JSInterpPtr interp, char *name, JSType *value);

/* Get the value of global variable <name> to <value>. */
void js_get_var (JSInterpPtr interp, char *name, JSType *value);


/*
 * Global methods.
 */

/*
 * Create a new global method to the interpreter <interp>.  The
 * method's name will be <name> and its implementation is function
 * <proc>.  The argument <context> is a user-specified data that is
 * passed to the command <proc>.  When the command is deleted, the
 * argument <context_free_proc> is called for data <context> to free
 * up all resources the <context> might have.  The function returns
 * 1 if the operation was successful or 0 otherwise.
 */
int js_create_global_method (JSInterpPtr interp, char *name,
			     JSGlobalMethodProc proc, void *context,
			     JSFreeProc context_free_proc);

/*
 * Call the module initialization function <init_proc>.  The function
 * return 1 if the module was successfully initialized or 0 otherwise.
 * In case of error, the error message can be fetched with the
 * js_error_mesage() function.
 */
int js_define_module (JSInterpPtr interp, JSModuleInitProc init_proc);

/*
 * The default modules.
 */

/* JavaScript interpreter extension. */
extern void js_ext_JS (JSInterpPtr interp);

/* Curses extension. */
extern void js_ext_curses (JSInterpPtr interp);

/* MD5 extension. */
extern void js_ext_MD5 (JSInterpPtr interp);

#endif /* not JS_H */
