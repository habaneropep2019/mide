/*
 * Optimized `switch' instruction dispatcher.
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
 * $Source: /home/kde/kdesupport/js/vmswitch.c,v $
 * $Id: vmswitch.c,v 1.1.1.1 1998/08/09 21:05:34 kulow Exp $
 */

#include "jsint.h"

/*
 * Types and definitions.
 */

#define SAVE_OP(a)				\
reloc[cp - fixed_code - 1] = &f->code[cpos];	\
f->code[cpos++].u.op = (a)

#define SAVE_INT8(a)	f->code[cpos++].u.i8 = (a)
#define SAVE_INT16(a)	f->code[cpos++].u.i16 = (a)
#define SAVE_INT32(a)	f->code[cpos++].u.i32 = (a)

#define ARG_INT32()	f->code[cpos].u.i32

#define READ_INT8(var)	(var) = (pc++)->u.i8
#define READ_INT16(var)	(var) = (pc++)->u.i16
#define READ_INT32(var)	(var) = (pc++)->u.i32

#define SETPC(ofs)		pc = (ofs)
#define SETPC_RELATIVE(ofs)	pc += (ofs)

#define CALL_USER_FUNC(f)	pc = ((Function *) (f))->code

#define DONE() goto done

#define ERROR(msg)		\
  do {				\
    JS_SAVE_REGS ();		\
    strcpy (vm->error, (msg));	\
    js_vm_error (vm);		\
    /* NOTREACHED */		\
  } while (0)


struct compiled_st
{
  union
  {
    void *ptr;
    JSUInt8 op;
    JSInt8 i8;
    JSInt16 i16;
    JSInt32 i32;
  } u;
};

typedef struct compiled_st Compiled;

struct function_st
{
  JSHeapDestroyableCB destroy;

  char *name;
  Compiled *code;
  unsigned int length;
};

typedef struct function_st Function;


/*
 * Prototypes for static functions.
 */

static void function_destroy (void *ptr);

static Function *link_code (JSVirtualMachine *vm, unsigned char *code,
			    unsigned int code_len,
			    unsigned int consts_offset);

static void execute_code (JSVirtualMachine *vm, Function *f,
			  unsigned int argc, JSNode *argv);


/*
 * Global functions.
 */

int
js_vm_switch_exec (JSVirtualMachine *vm, JSByteCode *bc, JSSymtabEntry *symtab,
		   unsigned int num_symtab_entries,
		   unsigned int consts_offset,
		   unsigned char *debug_info, unsigned int debug_info_len,
		   char *func_name, JSNode *func,
		   unsigned int argc, JSNode *argv)
{
  int i;
  unsigned int ui;
  Function *global_f = NULL;
  Function *f;
  unsigned char *code = NULL;

  if (bc)
    {
      /* Executing byte-code. */

      /* Find the code section. */
      for (i = 0; i < bc->num_sects; i++)
	if (bc->sects[i].type == JS_BCST_CODE)
	  code = bc->sects[i].data;
      assert (code != NULL);

      /* Enter all functions to the known functions of the VM. */
      for (i = 0; i < num_symtab_entries; i++)
	{
	  /* Link the code to our environment. */
	  f = link_code (vm, code + symtab[i].offset,
			 symtab[i + 1].offset - symtab[i].offset,
			 consts_offset);
	  f->name = js_strdup (vm, symtab[i].name);

	  if (strcmp (symtab[i].name, JS_GLOBAL_NAME) == 0)
	    global_f = f;
	  else
	    {
	      if (vm->verbose > 3)
		printf ("VM: link: %s(): start=%d, length=%d\n",
			symtab[i].name, symtab[i].offset,
			symtab[i + 1].offset - symtab[i].offset);
	      ui = js_vm_intern (vm, symtab[i].name);

	      vm->globals[ui].type = JS_FUNC;
	      vm->globals[ui].u.vfunction = js_vm_make_function (vm, f);
	    }
	}
    }
  else
    {
      JSNode *n;

      /* Applying arguments to function. */

      if (func_name)
	{
	  /* Lookup function. */
	  JSSymbol sym;

	  sym = js_vm_intern (vm, func_name);
	  n = &vm->globals[sym];
	  if (n->type != JS_FUNC)
	    {
	      sprintf (vm->error, "undefined function `%s' in apply",
		       func_name);
	      return 0;
	    }

	  if (vm->verbose > 1)
	    printf ("VM: calling %s()\n", func_name);
	}
      else
	{
	  /* Use the function the caller gave us. */
	  n = func;
	  if (n->type != JS_FUNC)
	    {
	      sprintf (vm->error, "illegal function in apply");
	      return 0;
	    }

	  if (vm->verbose > 1)
	    printf ("VM: calling function\n");
	}
      f = n->u.vfunction->implementation;

      execute_code (vm, f, argc, argv);
    }

  if (global_f)
    {
      if (vm->verbose > 1)
	printf ("VM: exec: %s\n", global_f->name);

      /* Execute. */
      execute_code (vm, global_f, 0, NULL);
    }

  return 1;
}


const char *
js_vm_switch_func_name (JSVirtualMachine *vm, void *program_counter)
{
  int i;
  Function *f;
  Compiled *pc = program_counter;
  JSNode *sp = vm->sp;

  /* Check the globals. */
  for (i = 0; i < vm->num_globals; i++)
    if (vm->globals[i].type == JS_FUNC)
      {
	f = (Function *) vm->globals[i].u.vfunction->implementation;
	if (f->code < pc && pc < f->code + f->length)
	  return f->name;
      }

  /* No luck.  Let's try the stack. */
  for (sp++; sp < vm->stack + vm->stack_size; sp++)
    if (sp->type == JS_FUNC)
      {
	f = (Function *) sp->u.vfunction->implementation;
	if (f->code < pc && pc < f->code + f->length)
	  return f->name;
      }

  /* Still no matches.  This shouldn't be reached... ok, who cares? */
  return JS_GLOBAL_NAME;
}


const char *
js_vm_switch_debug_position (JSVirtualMachine *vm,
			     unsigned int *linenum_return)
{
  /* XXX */
  return NULL;
}


/*
 * Static functions.
 */

static void
function_destroy (void *ptr)
{
  Function *f = ptr;

  js_free (f->name);
  js_free (f->code);
}


static Function *
link_code (JSVirtualMachine *vm, unsigned char *code, unsigned int code_len,
	   unsigned int consts_offset)
{
  unsigned char *cp, *end;
  JSInt32 i;
  Compiled **reloc;
  unsigned int cpos;
  Function *f;
  unsigned char *fixed_code;

  /* Terminate the code with op `done'. */
  fixed_code = js_malloc (vm, code_len + 1);
  memcpy (fixed_code, code, code_len);
  fixed_code[code_len] = 1;	/* op `done' */

  cp = fixed_code;
  end = fixed_code + code_len + 1;

  /* Alloc function closure. */
  f = js_vm_alloc_destroyable (vm, sizeof (*f));
  f->destroy = function_destroy;

  /* Allocate space for our compiled code.  <length> is enought. */
  f->code = js_malloc (vm, (code_len + 1) * sizeof (Compiled));
  reloc = js_calloc (vm, code_len + 1, sizeof (Compiled *));

  /* Link phase 1: constants and symbols. */
  cpos = 0;
  while (cp < end)
    {
      switch (*cp++)
	{
	  /* include c1switch.h */
#include "c1switch.h"
	  /* end include c1switch.h */
	}
    }
  f->length = cpos;

  /* Link phase 2: relative jumps. */
  cp = fixed_code;
  cpos = 0;
  while (cp < end)
    {
      switch (*cp++)
	{
	  /* include c2switch.h */
#include "c2switch.h"
	  /* end include c2switch.h */
	}
    }
  /* Handle debug info. */
  /* XXX */

  js_free (reloc);
  js_free (fixed_code);

  return f;
}


/*
 * Execute byte code by using the `switch' dispatch technique.
 */

static void
execute_code (JSVirtualMachine *vm, Function *f, unsigned int argc,
	      JSNode *argv)
{
  JSNode *sp;
  JSNode *fp;
  JSNode *function;
  JSNode builtin_result;
  Compiled *pc;
  JSInt32 i, j;
  JSInt8 i8;

  /* Create the initial stack frame by hand. */
  sp = vm->sp;

  /* Protect the function from gc. */
  JS_SP0->type = JS_FUNC;
  JS_SP0->u.vfunction = js_vm_make_function (vm, f);
  JS_PUSH ();

  /* Push arguments to the stack. */
  i = argc;
  for (i--; i >= 0; i--)
    {
      JS_COPY (JS_SP0, &argv[i]);
      JS_PUSH ();
    }

  /* Empty this pointer. */
  JS_SP0->type = JS_NULL;
  JS_PUSH ();

  /* Init fp and pc so our SUBROUTINE_CALL will work. */
  fp = NULL;
  pc = NULL;

  JS_SUBROUTINE_CALL (f);

  /* Ok, now we are ready to run. */

  while (1)
    {
      switch ((pc++)->u.op)
	{
	  /* include eswitch.h */
#include "eswitch.h"
	  /* end include eswitch.h */

	default:
	  fprintf (stderr, "execute_code: unknown opcode %d\n",
		   (pc - 1)->u.op);
	  exit (1);
	  break;
	}
    }

 done:

  /* All done. */
  JS_COPY (&vm->exec_result, JS_SP1);
}
