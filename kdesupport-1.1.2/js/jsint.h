/*
 * Internal definitions for the JavaScript interpreter.
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
 * $Source: /home/kde/kdesupport/js/jsint.h,v $
 * $Id: jsint.h,v 1.2.2.1 1999/07/15 15:04:37 porten Exp $
 */

#ifndef JSINT_H
#define JSINT_H

/* We have always config.h */
#include <config.h>

#include <stdio.h>
#include <assert.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if STDC_HEADERS
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <float.h>

#else /* not STDC_HEADERS */

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_FLOAT_H
#include <float.h>
#endif

#endif /* not STDC_HEADERS */

#include "js.h"

/*
 * Types and definitions.
 */

#define JS_BC_FILE_MAGIC	0xc0014a53

#define JS_GLOBAL_NAME	".global"

#define JS_SYMBOL_NULL	((JSSymbol) -1)

/*
 * Read macros for byte code files.
 */

#define JS_BC_READ_INT32(cp, var)	\
  (var) = (cp)[0];			\
  (var) <<= 8;				\
  (var) |= (cp)[1];			\
  (var) <<= 8;				\
  (var) |= (cp)[2];			\
  (var) <<= 8;				\
  (var) |= (cp)[3]

#define JS_BC_READ_INT16(cp, var)	\
  (var) = (cp)[0];			\
  (var) <<= 8;				\
  (var) |= (cp)[1]

#define JS_BC_READ_INT8(cp, var)	\
  (var) = (cp)[0]

#define JS_BC_WRITE_INT32(cp, var)			\
  cp[3] = (unsigned char) ((var) & 0x000000ff);		\
  cp[2] = (unsigned char) (((var) >> 8) & 0x000000ff);	\
  cp[1] = (unsigned char) (((var) >> 16) & 0x000000ff); \
  cp[0] = (unsigned char) (((var) >> 24) & 0x000000ff)


/* General VM macros. */

/* STACKFRAME */

#define JS_SP0		sp
#define JS_SP1		(sp + 1)
#define JS_SP2		(sp + 2)
#define JS_SP(n)	(sp + (n))

#define JS_LOCAL(n)	(fp - 3 - (n))
#define JS_ARG(n)	(fp + 1 + (n))

#define JS_WITHPTR	(fp - 1)

#define JS_PUSH()	sp--
#define JS_POP()	sp++

#define JS_COPY(to, from)		\
  do {					\
    (to)->type = (from)->type;		\
    (to)->u.copy.a = (from)->u.copy.a;	\
    (to)->u.copy.b = (from)->u.copy.b;	\
  } while (0)

#define JS_CONST(n)	(&vm->consts[(n)])
#define JS_GLOBAL(n)	(&vm->globals[(n)])

#define JS_SAVE_REGS()		\
  do {				\
    vm->sp = sp;		\
    vm->pc = pc;		\
  } while (0)

#define JS_MAYBE_GC()					\
  do {							\
    if (vm->gc.bytes_allocated >= vm->gc.trigger)	\
      js_vm_garbage_collect (vm, fp, sp);		\
  } while (0)

#define JS_IS_TRUE(n) ((n)->type > JS_INTEGER				\
		    || ((n)->type == JS_BOOLEAN && (n)->u.vboolean)	\
		    || ((n)->type == JS_INTEGER && (n)->u.vinteger))

#define JS_IS_FALSE(n) ((n)->type < JS_BOOLEAN				\
		     || ((n)->type == JS_BOOLEAN && !(n)->u.vboolean)	\
		     || ((n)->type == JS_INTEGER && !(n)->u.vinteger))

#define JS_SUBROUTINE_CALL(function)		\
  do {						\
    /* STACKFRAME */				\
						\
    /* Save frame pointer. */			\
    JS_SP0->type = JS_IPTR;			\
    JS_SP0->u.iptr = fp;			\
						\
    /* Update fp. */				\
    fp = JS_SP0;				\
    JS_PUSH ();					\
						\
    /* Insert empty with pointer. */		\
    JS_SP0->type = JS_IPTR;			\
    JS_SP0->u.iptr = NULL;			\
    JS_PUSH ();					\
						\
    /* Save return address. */			\
    JS_SP0->type = JS_IPTR;			\
    JS_SP0->u.iptr = pc;			\
    JS_PUSH ();					\
						\
    /* And finally, jump to the method code. */ \
    CALL_USER_FUNC ((function));		\
  } while (0)


/* Flags for heap memory block. */
#define JS_HEAP_BLOCK_MARK		0x01
#define JS_HEAP_BLOCK_DESTROYABLE	0x02

#define JS_NUM_HEAP_FREELISTS	20

/*
 * Integer types.
 */

typedef unsigned char JSUInt8;
typedef signed char JSInt8;

typedef unsigned short JSUInt16;
typedef short JSInt16;

#if SIZEOF_INT == 4

typedef unsigned int JSUInt32;
typedef int JSInt32;

#else /* not SIZEOF_INT == 4 */

#if SIZEOF_LONG == 4

typedef unsigned long JSUInt32;
typedef long JSInt32;

#else /* not SIZEOF_LONG == 4 */

#error "do not know how to define a 32 bit long integer"

#endif /* not SIZEOF_LONG == 4 */

#endif /* not SIZEOF_INT == 4 */

/* The destroy callback for the destroyable heap blocks. */
typedef void (*JSHeapDestroyableCB) (void *ptr);

/*
 * Each destroyable heap block must be castable to this structure e.g.
 * the first item in the block must be pointer to the destroy function.
 */
struct js_heap_destroyable_st
{
  JSHeapDestroyableCB destroy;
};

typedef struct js_heap_destroyable_st JSHeapDestroyable;


/* Interned symbol. */
typedef unsigned int JSSymbol;

/* JavaScript Types. */

typedef enum
{
  JS_UNDEFINED	= 0,
  JS_NULL	= 1,
  JS_BOOLEAN 	= 2,
  JS_INTEGER 	= 3,		/* Integer, float and nan are `number' */
  JS_STRING 	= 4,
  JS_FLOAT 	= 5,
  JS_ARRAY	= 6,
  JS_OBJECT	= 7,

  /*
   * The following ones are the internal types, used by this implementation.
   */

  JS_SYMBOL 	= 10,
  JS_BUILTIN 	= 11,
  JS_FUNC	= 12,
  JS_NAN	= 13,

  JS_IPTR 	= 14
} JSNodeType;

struct js_node_st;
struct js_vm_st;
struct js_builtin_info_st;

/* Registry information for builtin objects. */

#define JS_PROPERTY_FOUND	1
#define JS_PROPERTY_UNKNOWN	0

typedef void (*JSBuiltinGlobalMethod) (struct js_vm_st *vm,
				       struct js_builtin_info_st *builtin_info,
				       void *instance_context,
				       struct js_node_st *result_return,
				       struct js_node_st *args);

/*
 * Function to call method <method> from the object.  Function must return
 * JS_PROPERTY_FOUND if the method was found or JS_PROPERTY_UNKNOWN
 * otherwise.
 */
typedef int (*JSBuiltinMethod) (struct js_vm_st *vm,
				struct js_builtin_info_st *builtin_info,
				void *instance_context,
				JSSymbol method,
				struct js_node_st *result_return,
				struct js_node_st *args);

/*
 * Function to load and set property <property> of object.  If <set>
 * is true, property <property> should be set to value <node>.  Otherwise
 * function should return the value of property <property> in <node>.
 * Function must return JS_PROPERTY_FOUND if the property was found or
 * JS_PROPERTY_UNKNOWN otherwise.
 */
typedef int (*JSBuiltinProperty) (struct js_vm_st *vm,
				  struct js_builtin_info_st *builtin_info,
				  void *instance_context,
				  JSSymbol property, int set,
				  struct js_node_st *node);

typedef void (*JSBuiltinNew) (struct js_vm_st *vm,
			      struct js_builtin_info_st *builtin_info,
			      struct js_node_st *args,
			      struct js_node_st *result_return);

typedef void (*JSBuiltinDelete) (struct js_builtin_info_st *builtin_info,
				 void *instance_context);

typedef void (*JSBuiltinMark) (struct js_builtin_info_st *builtin_info,
			       void *instance_context);

typedef void (*JSBuiltinObjectCtxDelete) (void *obj_context);

struct js_builtin_info_st
{
  JSHeapDestroyableCB	destroy;

  JSBuiltinGlobalMethod	global_method_proc;
  JSBuiltinMethod 	method_proc;
  JSBuiltinProperty	property_proc;
  JSBuiltinNew		new_proc;
  JSBuiltinDelete	delete_proc;
  JSBuiltinMark		mark_proc;

  void *obj_context;
  JSBuiltinObjectCtxDelete obj_context_delete;

  struct js_object_st *prototype;
};

typedef struct js_builtin_info_st JSBuiltinInfo;

/* Builtin object / class. */
struct js_builtin_st
{
  JSHeapDestroyableCB destroy;

  JSBuiltinInfo *info;
  void *instance_context;

  struct js_object_st *prototype;
};

typedef struct js_builtin_st JSBuiltin;

/* String. */
struct js_string_st
{
  /* Flags. */
  unsigned int staticp : 1;

  unsigned char *data;
  unsigned int len;

  struct js_object_st *prototype;
};

typedef struct js_string_st JSString;

/* Array. */
struct js_array_st
{
  unsigned int length;
  struct js_node_st *data;

  struct js_object_st *prototype;
};

typedef struct js_array_st JSArray;

/* Function. */
struct js_function_st
{
  void *implementation;
  struct js_object_st *prototype;
};

typedef struct js_function_st JSFunction;

/* Node. */
struct js_node_st
{
  JSNodeType type;

  union
  {
    unsigned int vboolean;

    JSString *vstring;

    long vinteger;
    double vfloat;

    struct js_object_st *vobject;

    JSArray *varray;

    /* Internal values. */

    JSSymbol vsymbol;

    JSBuiltin *vbuiltin;

    JSFunction *vfunction;

    void *iptr;

    struct
    {
      unsigned long a;
      unsigned long b;
    } copy;
  } u;
};

typedef struct js_node_st JSNode;


/* Object. */

/* Hash node for object's properties. */
struct js_object_prop_hash_bucket_st
{
  struct js_object_prop_hash_bucket_st *next;
  unsigned char *data;
  unsigned int len;
  unsigned int value;
};

typedef struct js_object_prop_hash_bucket_st JSObjectPropHashBucket;

/* The attribute flags for object's properties. */
#define JS_ATTRIB_READONLY		1
#define JS_ATTRIB_DONTENUM		2
#define JS_ATTRIB_DONTDELETE		4
#define JS_ATTRIB_Internal		8

/* Object's property. */
struct js_property_st
{
  JSSymbol name;
  JSNode value;
  unsigned int attributes;
};

typedef struct js_property_st JSProperty;

struct js_object_st
{
  JSObjectPropHashBucket **hash;
  unsigned int *hash_lengths;
  unsigned int num_props;	/* Number of properties in this object. */
  JSProperty *props;
};

typedef struct js_object_st JSObject;


/* Byte code. */

typedef enum
{
  JS_BCST_CODE = 0,
  JS_BCST_CONSTANTS = 1,
  JS_BCST_SYMTAB = 2,
  JS_BCST_DEBUG = 3
} JSBCSectionType;

struct js_bc_sect_st
{
  JSBCSectionType type;
  unsigned int length;
  void *data;			/* <length> bytes of data */
};

typedef struct js_bc_sect_st JSBCSect;

struct js_bc_st
{
  unsigned int num_sects;
  JSBCSect *sects;
};

typedef struct js_bc_st JSByteCode;

/* Debug information. */
#define JS_DI_FILENAME		1
#define JS_DI_LINENUMBER 	2

/* Heap block. */
struct js_heap_block_st
{
  struct js_heap_block_st *next;
  unsigned int size;
  /* <size> bytes of data follows the structure. */
};

typedef struct js_heap_block_st JSHeapBlock;

/* Heap memory block. */

#define JS_MEM_DEBUG 0

struct js_heap_memory_block_st
{
#if JS_MEM_DEBUG
  unsigned int magic;
#endif
  struct js_heap_memory_block_st *next;
  unsigned int flags;
  unsigned int size;
  /* <size> bytes of data follows this header. */
};

typedef struct js_heap_memory_block_st JSHeapMemoryBlock;

/* Parsed symbol table entry. */
struct js_symtab_entry_st
{
  char *name;
  unsigned int offset;
};

typedef struct js_symtab_entry_st JSSymtabEntry;


/*
 * Entry points to different byte-code instruction dispatcher functions.
 * Each dispatcher must implement these.
 */

typedef int (*JSVMExecute) (struct js_vm_st *vm, JSByteCode *bc,
			    JSSymtabEntry *symtab,
			    unsigned int num_symtab_entries,
			    unsigned int consts_offset,
			    unsigned char *debug_info,
			    unsigned int debug_info_len,
			    char *func_name, JSNode *func,
			    unsigned int argc, JSNode *argv);

typedef const char *(*JSVMFuncName) (struct js_vm_st *vm, void *pc);

typedef const char *(*JSVMDebugPosition) (struct js_vm_st *vm,
					  unsigned int *linenum_return);


/* Virtual Machine. */

#define JS_HASH_TABLE_SIZE 256

struct js_hash_bucket_st
{
  struct js_hash_bucket_st *next;
  char *name;
  union
  {
    void *data;
    unsigned int ui;
  } u;
};

typedef struct js_hash_bucket_st JSHashBucket;

/* Error handler frame. */
struct js_error_handler_frame_st
{
  struct js_error_handler_frame_st *next;
  jmp_buf error_jmp;

  /* The value thrown by the throw operand. */
  JSNode thrown;

  /* Saved state for the `try_push' operand. */
  JSNode *sp;
  JSNode *fp;
  void *pc;
  JSInt32 pc_delta;
};

typedef struct js_error_handler_frame_st JSErrorHandlerFrame;

struct js_vm_st
{
  /* Options for the virtual machine. */
  unsigned int verbose;		/* verbose has different levels. */

  unsigned int stacktrace_on_error : 1;
  unsigned int warn_undef : 1;

  /* The default system streams. */
  FILE *s_stdin;
  FILE *s_stdout;
  FILE *s_stderr;

  /* The byte-code instruction dispatcher. */
  JSVMDispatchMethod	dispatch_method;
  const char 	       *dispatch_method_name;
  JSVMExecute 		dispatch_execute;
  JSVMFuncName		dispatch_func_name;
  JSVMDebugPosition	dispatch_debug_position;

  /* Constants pool. */
  JSNode *consts;
  unsigned int num_consts;
  unsigned int consts_alloc;

  /*
   * Global symbols (both functions and variables).  <globals_hash> is
   * a name-to-index mapping between symbol names and their positions
   * in <globals>.
   */
  JSHashBucket *globals_hash[JS_HASH_TABLE_SIZE];
  JSNode *globals;
  unsigned int num_globals;
  unsigned int globals_alloc;

  /* Stack. */
  JSNode *stack;
  unsigned int stack_size;
  JSNode *sp;			/* Fuzzy stack pointer. */

  void *pc;			/* Fuzzy program counter. */

  /* Builtin objects for the primitive datatypes. */
  JSBuiltinInfo *prim[JS_IPTR + 1];

  /* Some commonly used symbols. */
  struct
  {
    JSSymbol s___proto__;
    JSSymbol s_prototype;
    JSSymbol s_toString;
  } syms;

  /* Heap. */

  JSHeapBlock *heap;
  JSHeapMemoryBlock *heap_freelists[JS_NUM_HEAP_FREELISTS];
  unsigned long heap_size;

  /* Information for the garbage collector. */
  struct
  {
    unsigned long trigger;
    unsigned long bytes_allocated;
    unsigned long bytes_free;
    unsigned long count;
  } gc;

  /* Error handler frames. */
  JSErrorHandlerFrame *error_handler;

  /* Buffer for the error message.  Sorry, we don't support long errors ;-) */
  char error[1024];

  /*
   * The result from the latest evaluation.  This is set when the
   * js_vm_execute() or js_vm_apply() functions return to the caller.
   */
  JSNode exec_result;

#if PROFILING

  /* Byte-code operand profiling support. */

  unsigned int prof_count[256];
  unsigned char prof_op;

#endif /* PROFILING */
};

typedef struct js_vm_st JSVirtualMachine;


/*
 * Global variables.
 */

extern unsigned char js_latin1_tolower[256];
extern unsigned char js_latin1_toupper[256];

/*
 * Prototypes for global functions.
 */

/*
 * Memory allocation routines.  If the allocation request fails, the
 * error recovery is performed according to the argument <vm>.  If
 * <vm> is not NULL, an error message is formatted to vm->error and an
 * error is raise with js_vm_error().  If the <vm> is NULL, the
 * functions will return value NULL.  It is an error to call these
 * functions with a non-NULL <vm> that has no error handler
 * initialized.
 */

#ifndef JS_DEBUG_MEMORY_LEAKS
#define JS_DEBUG_MEMORY_LEAKS 0
#endif /* not JS_DEBUG_MEMORY_LEAKS */

#if JS_DEBUG_MEMORY_LEAKS

#define js_malloc(vm, size)		js_malloc_i ((vm), (size), \
						     __FILE__, __LINE__)
#define js_calloc(vm, num, size)	js_calloc_i ((vm), (num), (size), \
						     __FILE__, __LINE__)
#define js_realloc(vm, ptr, size) 	js_realloc_i ((vm), (ptr), (size), \
						      __FILE__, __LINE__)
#define js_strdup(vm, str)		js_strdup_i ((vm), (str), \
						     __FILE__, __LINE__)

void *js_malloc_i (JSVirtualMachine *vm, size_t size, char *, int);
void *js_calloc_i (JSVirtualMachine *vm, size_t num, size_t size, char *, int);
void *js_realloc_i (JSVirtualMachine *vm, void *ptr, size_t size, char *, int);
void js_free (void *ptr);
char *js_strdup_i (JSVirtualMachine *vm, char *str, char *, int);

#else /* not JS_DEBUG_MEMORY_LEAKS */

void *js_malloc (JSVirtualMachine *vm, size_t size);
void *js_calloc (JSVirtualMachine *vm, size_t num, size_t size);
void *js_realloc (JSVirtualMachine *vm, void *ptr, size_t size);
void js_free (void *ptr);
char *js_strdup (JSVirtualMachine *vm, char *str);

#endif /* not JS_DEBUG_MEMORY_LEAKS */

/* Byte code. */

JSByteCode *js_bc_read_file (FILE *fp);

JSByteCode *js_bc_read_data (unsigned char *data, unsigned int datalen);

void js_bc_free (JSByteCode *bc);


/* Virtual machine. */

JSVirtualMachine *js_vm_create (unsigned int stack_size,
				JSVMDispatchMethod dispatch_method,
				unsigned int verbose, int stacktrace_on_error,
				FILE *s_stdin, FILE *s_stdout, FILE *s_stderr);

void js_vm_destroy (JSVirtualMachine *vm);

/*
 * Execute byte code <bc>.  Function returns 1 if the operation was
 * successful or 0 if any errors were encountered.  In case of errors,
 * the error message is stored at vm->error.
 */
int js_vm_execute (JSVirtualMachine *vm, JSByteCode *bc);

/*
 * Apply function <func_name> to arguments <argc, argv>.  If
 * function's name <func_name> is NULL, then <func> must specify function
 * to which arguments are applied.
 */
int js_vm_apply (JSVirtualMachine *vm, char *func_name, JSNode *func,
		 unsigned int argc, JSNode *argv);

/* Map program counter to the source file line. */
const char *js_vm_debug_position (JSVirtualMachine *vm,
				  unsigned int *linenum_return);

/* Fetch the function name from the program counter value. */
const char *js_vm_func_name (JSVirtualMachine *vm, void *pc);

/* Intern symbol <name> to virtual machine and returns its JSSymbol id. */
JSSymbol js_vm_intern (JSVirtualMachine *vm, char *name);

/* Return the name of symbol <sym>. */
const char *js_vm_symname (JSVirtualMachine *vm, JSSymbol sym);

/*
 * Convert node <n> to its string presentations and return the result
 * in <result_return>.
 */
void js_vm_to_string (JSVirtualMachine *vm, const JSNode *n,
		      JSNode *result_return);


/*
 * Raise an error.  The error message must have been saved to vm->error
 * before this function is called.  The function never returns.
 */
void js_vm_error (JSVirtualMachine *vm);

/*
 * Count a hash value for <data_len> bytes of data <data>.  The resulting
 * hash value should be re-mapped to the correct range, for example,
 * with the mod operand.
 */
static unsigned int
js_count_hash (const char *data, unsigned int data_len)
{
  unsigned int val = 0, i;

  for (i = 0; i < data_len; i++)
    val = (val << 5) ^ (unsigned char) data[i]
      ^ (val >> 16) ^ (val >> 7);

  return val;
}


/* Prototypes for the different instruction dispatcher implementations. */

#if ALL_DISPATCHERS

int js_vm_switch0_exec (JSVirtualMachine *vm, JSByteCode *bc,
			JSSymtabEntry *symtab,
			unsigned int num_symtab_entries,
			unsigned int consts_offset,
			unsigned char *debug_info,
			unsigned int debug_info_len,
			char *func_name, JSNode *func,
			unsigned int argc, JSNode *argv);

const char *js_vm_switch0_func_name (JSVirtualMachine *vm, void *pc);

const char *js_vm_switch0_debug_position (JSVirtualMachine *vm,
					  unsigned int *linenum_return);

#endif /* ALL_DISPATCHERS */

int js_vm_switch_exec (JSVirtualMachine *vm, JSByteCode *bc,
		       JSSymtabEntry *symtab,
		       unsigned int num_symtab_entries,
		       unsigned int consts_offset,
		       unsigned char *debug_info, unsigned int debug_info_len,
		       char *func_name, JSNode *func,
		       unsigned int argc, JSNode *argv);

const char *js_vm_switch_func_name (JSVirtualMachine *vm, void *pc);

const char *js_vm_switch_debug_position (JSVirtualMachine *vm,
					 unsigned int *linenum_return);

int js_vm_jumps_exec (JSVirtualMachine *vm, JSByteCode *bc,
		      JSSymtabEntry *symtab,
		      unsigned int num_symtab_entries,
		      unsigned int consts_offset,
		      unsigned char *debug_info, unsigned int debug_info_len,
		      char *func_name, JSNode *func,
		      unsigned int argc, JSNode *argv);

const char *js_vm_jumps_func_name (JSVirtualMachine *vm, void *pc);

const char *js_vm_jumps_debug_position (JSVirtualMachine *vm,
					unsigned int *linenum_return);


/* Heap. */

void *js_vm_alloc (JSVirtualMachine *vm, unsigned int size);

void *js_vm_alloc_destroyable (JSVirtualMachine *vm, unsigned int size);

void *js_vm_realloc (JSVirtualMachine *vm, void *ptr, unsigned int new_size);

void js_vm_free (JSVirtualMachine *vm, void *ptr);

void js_vm_garbage_collect (JSVirtualMachine *vm, JSNode *fp, JSNode *sp);

void js_vm_clear_heap (JSVirtualMachine *vm);

void js_vm_mark (JSNode *node);

int js_vm_mark_ptr (void *ptr);

int js_vm_is_marked_ptr (void *ptr);

/* Function. */

static JSFunction *
js_vm_make_function (JSVirtualMachine *vm, void *implementation)
{
  JSFunction *f = js_vm_alloc (vm, sizeof (*vm));

  f->implementation = implementation;
  f->prototype = NULL;

  return f;
}


/* Built-in. */

/* Create a new built-in info. */
JSBuiltinInfo *js_vm_builtin_info_create (JSVirtualMachine *vm);

/* Create a new builtin object with <info, instance_context> to <result>. */
void js_vm_builtin_create (JSVirtualMachine *vm, JSNode *result,
			   JSBuiltinInfo *info, void *instance_context);

/* Array. */

static void
js_vm_make_array (JSVirtualMachine *vm, JSNode *n, unsigned int length)
{
  unsigned int i;

  n->type = JS_ARRAY;
  n->u.varray = js_vm_alloc (vm, sizeof (*n->u.varray));
  n->u.varray->prototype = NULL;
  n->u.varray->length = length;
  n->u.varray->data = js_vm_alloc (vm, length * sizeof (JSNode));

  for (i = 0; i < length; i++)
    n->u.varray->data[i].type = JS_UNDEFINED;
}

static void
js_vm_expand_array (JSVirtualMachine *vm, JSNode *n, unsigned int length)
{
  if (n->u.varray->length < length)
    {
      n->u.varray->data = js_vm_realloc (vm, n->u.varray->data,
					 length * sizeof (JSNode));
      for (; n->u.varray->length < length; n->u.varray->length++)
	n->u.varray->data[n->u.varray->length].type = JS_UNDEFINED;
    }
}

/* File. */

/* Enter file <fp> to the system. */
void js_builtin_File_new (JSVirtualMachine *vm, JSNode *result_return,
			  char *path, FILE *fp, int pipe, int dont_close);

/* RegExp. */

/*
 * Do search-replace for the string <data, datalen> by replacing
 * matches of <regexp> with <repl, repl_len>.  The resulting string is
 * returned in <result_return>
 */
void js_builtin_RegExp_replace (JSVirtualMachine *vm, char *data,
				unsigned int datalen, JSNode *regexp,
				char *repl, unsigned int repl_len,
				JSNode *result_return);

/*
 * Do regexp match against <data, datalen>.   Format the result array
 * to <result_return>.
 */
void js_builtin_RegExp_match (JSVirtualMachine *vm, char *data,
			      unsigned int datalen, JSNode *regexp,
			      JSNode *result_return);

/*
 * Do regexp search against <data, datalen>.  Return the start index of
 * the match in <result_return>.
 */
void js_builtin_RegExp_search (JSVirtualMachine *vm, char *data,
			       unsigned int datalen, JSNode *regexp,
			       JSNode *result_return);

/*
 * Split the string <data, datalen> by regular expression <regexp>.
 * Function returns an array containing the substrings.
 */
void js_builtin_RegExp_split (JSVirtualMachine *vm, char *data,
			      unsigned int datalen, JSNode *regexp,
			      unsigned int limit, JSNode *result_return);

/* Object. */

JSObject *js_vm_object_new (JSVirtualMachine *vm);

void js_vm_object_mark (JSObject *obj);

int js_vm_object_load_property (JSVirtualMachine *vm, JSObject *obj,
				JSSymbol prop, JSNode *value_return);

void js_vm_object_store_property (JSVirtualMachine *vm, JSObject *obj,
				  JSSymbol prop, JSNode *value);

void js_vm_object_delete_property (JSVirtualMachine *vm, JSObject *obj,
				   JSSymbol prop);

void js_vm_object_load_array (JSVirtualMachine *vm, JSObject *obj, JSNode *sel,
			      JSNode *value_return);

void js_vm_object_store_array (JSVirtualMachine *vm, JSObject *obj,
			       JSNode *sel, JSNode *value);

void js_vm_object_delete_array (JSVirtualMachine *vm, JSObject *obj,
				JSNode *sel);

int js_vm_object_nth (JSVirtualMachine *vm, JSObject *obj, int nth,
		      JSNode *value_return);


/* Debug. */

void js_vm_stacktrace (JSVirtualMachine *vm, unsigned int num_frames);


/* Strings. */

static void
js_vm_make_string (JSVirtualMachine *vm, JSNode *n, const char *data,
		   unsigned int data_len)
{
  n->type = JS_STRING;
  n->u.vstring = js_vm_alloc (vm, sizeof (*n->u.vstring));
  n->u.vstring->staticp = 0;
  n->u.vstring->prototype = NULL;
  n->u.vstring->len = data_len;
  n->u.vstring->data = js_vm_alloc (vm, data_len);
  if (data)
    memcpy (n->u.vstring->data, data, data_len);

}


static void
js_vm_make_static_string (JSVirtualMachine *vm, JSNode *n, const char *data,
			  unsigned int data_len)
{
  n->type = JS_STRING;
  n->u.vstring = js_vm_alloc (vm, sizeof (*n->u.vstring));
  n->u.vstring->staticp = 1;
  n->u.vstring->prototype = NULL;
  n->u.vstring->len = data_len;
  n->u.vstring->data = (unsigned char *) data;
}


static int
js_compare_strings (JSNode *a, JSNode *b)
{
  unsigned int i;

  for (i = 0; i < a->u.vstring->len && i < b->u.vstring->len; i++)
    {
      if (a->u.vstring->data[i] < b->u.vstring->data[i])
	return -1;
      if (a->u.vstring->data[i] > b->u.vstring->data[i])
	return 1;
    }
  if (a->u.vstring->len < b->u.vstring->len)
    return -1;
  if (a->u.vstring->len > b->u.vstring->len)
    return 1;

  return 0;
}


static char *
js_string_to_c_string (JSVirtualMachine *vm, JSNode *a)
{
  char *cp;

  cp = js_malloc (vm, a->u.vstring->len + 1);
  memcpy (cp, a->u.vstring->data, a->u.vstring->len);
  cp[a->u.vstring->len] = '\0';

  return cp;
}


/* Misc helper functions. */

unsigned long js_crc32 (const unsigned char *s, unsigned int len);


/*
 * Definitions for the JavaScript part of the JavaScript interp.
 */

/* Flags for the compiler.  See `jsc/entry.js'. */

#define JSC_FLAG_VERBOSE			0x00000001
#define JSC_FLAG_ANNOTATE_ASSEMBLER		0x00000002
#define JSC_FLAG_GENERATE_DEBUG_INFO		0x00000004

#define JSC_FLAG_OPTIMIZE_PEEPHOLE		0x00000020
#define JSC_FLAG_OPTIMIZE_JUMPS			0x00000040
#define JSC_FLAG_OPTIMIZE_BC_SIZE		0x00000080
#define JSC_FLAG_OPTIMIZE_HEAVY			0x00000100

#define JSC_FLAG_OPTIMIZE_MASK			0x0000fff0

#define JSC_FLAG_WARN_UNUSED_ARGUMENT		0x00010000
#define JSC_FLAG_WARN_UNUSED_VARIABLE		0x00020000
#define JSC_FLAG_WARN_SHADOW			0x00040000
#define JSC_FLAG_WARN_WITH_CLOBBER		0x00080000

#define JSC_FLAG_WARN_MASK			0xffff0000

/* JavaScript interpreter handle. */
struct js_interp_st
{
  JSInterpOptions options;
  JSVirtualMachine *vm;
};

/* Declaration for the JS compiler byte-code. */
extern unsigned char js_compiler_bytecode[];
extern unsigned int js_compiler_bytecode_len;

#endif /* not JSINT_H */
