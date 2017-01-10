/*
 * JavaScript interpreter main glue.
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
 * $Source: /home/kde/kdesupport/js/js.c,v $
 * $Id: js.c,v 1.1.1.1 1998/08/09 21:05:34 kulow Exp $
 */

#include "js.h"
#include "jsint.h"

/*
 * Types and definitions.
 */

/* Context for js_global_method_stub. */
struct js_global_method_context_st
{
  JSGlobalMethodProc proc;
  void *context;
  JSFreeProc free_proc;
  JSInterpPtr interp;
};

typedef struct js_global_method_context_st JSGlobalMethodContext;

/*
 * Prototypes for static functions.
 */

/* The module for JS' core global methods. */
static void js_core_globals (JSInterpPtr interp);

/*
 * Helper function to evaluate source <source> with compiler function
 * <compiler_function>.
 */
static int js_eval_source (JSInterpPtr interp, JSNode *source,
			   char *compiler_function);

/*
 * The stub function for global methods, create with the
 * js_create_global_method() API function.
 */
static void js_global_method_stub (JSVirtualMachine *vm,
				   JSBuiltinInfo *builtin_info,
				   void *instance_context,
				   JSNode *result_return,
				   JSNode *args);

/*
 * Global functions.
 */

void
js_init_default_options (JSInterpOptions *options)
{
  memset (options, 0, sizeof (*options));

  options->stack_size = 2048;
  options->dispatch_method = JS_VM_DISPATCH_JUMPS;

  options->warn_undef = 1;

  options->optimize_peephole = 1;
  options->optimize_jumps_to_jumps = 1;

  /* The default system streams. */
  options->s_stdin = stdin;
  options->s_stdout = stdout;
  options->s_stderr = stderr;
}


JSInterpPtr
js_create_interp (JSInterpOptions *options)
{
  JSInterpPtr interp;
  JSByteCode *bc;
  JSInterpOptions default_options;

  interp = js_calloc (NULL, 1, sizeof (*interp));
  if (interp == NULL)
    return NULL;

  if (options == NULL)
    {
      js_init_default_options (&default_options);
      options = &default_options;
    }

  memcpy (&interp->options, options, sizeof (*options));

  /* Create virtual machine. */
  interp->vm = js_vm_create (options->stack_size,
			     options->dispatch_method,
			     options->verbose,
			     options->stacktrace_on_error,
			     options->s_stdin,
			     options->s_stdout,
			     options->s_stderr);
  if (interp->vm == NULL)
    {
      js_free (interp);
      return NULL;
    }

  if (!options->no_compiler)
    {
      /* Define compiler to the virtual machine. */
      bc = js_bc_read_data (js_compiler_bytecode, js_compiler_bytecode_len);
      js_vm_execute (interp->vm, bc);
      js_bc_free (bc);
    }

  /* Initialize our extensions. */
  if (!js_define_module (interp, js_core_globals))
    {
      js_vm_destroy (interp->vm);
      js_free (interp);
      return NULL;
    }

  /* Ok, we'r done. */

  return interp;
}


void
js_destroy_interp (JSInterpPtr interp)
{
  js_vm_destroy (interp->vm);
  js_free (interp);
}


const char *
js_error_message (JSInterpPtr interp)
{
  return interp->vm->error;
}


int
js_eval (JSInterpPtr interp, char *code)
{
  JSNode source;

  js_vm_make_static_string (interp->vm, &source, code, strlen (code));
  return js_eval_source (interp, &source, "JSC$compile_string");
}


int
js_eval_data (JSInterpPtr interp, char *data, unsigned int datalen)
{
  JSNode source;

  js_vm_make_static_string (interp->vm, &source, data, datalen);
  return js_eval_source (interp, &source, "JSC$compile_string");
}


int
js_eval_file (JSInterpPtr interp, char *filename)
{
  char *cp;
  int result;

  cp = strrchr (filename, '.');
  if (cp && strcmp (cp, ".jsc") == 0)
    {
    run_bytecode:
      result = js_execute_byte_code_file (interp, filename);
    }
  else if (cp && strcmp (cp, ".js") == 0)
    {
    try_javascript:
      result = js_eval_javascript_file (interp, filename);
    }
  else
    {
      FILE *fp;

      /* Must look into the file. */

      fp = fopen (filename, "r");
      if (fp)
	{
	  int ch;

	  if ((ch = getc (fp)) == '#')
	    {
	      /* Skip the first sh-command line. */
	      while ((ch = getc (fp)) != EOF && ch != '\n')
		;
	      if (ch == EOF)
		{
		  fclose (fp);
		  goto try_javascript;
		}
	    }
	  else
	    ungetc (ch, fp);

	  /* Check if we can read the file magic. */
	  ch = getc (fp);
	  if (ch == 0xc0)
	    {
	      ch = getc (fp);
	      if (ch == 0x01)
		{
		  ch = getc (fp);
		  if (ch == 'J')
		    {
		      ch = getc (fp);
		      if (ch == 'S')
			{
			  /* Got it.  We find a valid byte-code file magic. */
			  fclose (fp);
			  goto run_bytecode;
			}
		    }
		}
	    }

	  fclose (fp);
	  /* FALLTHROUGH */
	}

      /*
       * If nothing else helps, we assume that the file contains JavaScript
       * source code that must be compiled.
       */
      goto try_javascript;
    }

  return result;
}


int
js_eval_javascript_file (JSInterpPtr interp, char *filename)
{
  JSNode source;

  js_vm_make_static_string (interp->vm, &source, filename, strlen (filename));
  return js_eval_source (interp, &source, "JSC$compile_file");
}


int
js_execute_byte_code_file (JSInterpPtr interp, char *filename)
{
  JSByteCode *bc;
  FILE *fp;
  int result;

  fp = fopen (filename, "rb");
  if (fp == NULL)
    {
      /* Let's borrow vm's error buffer. */
      sprintf (interp->vm->error, "couldn't open byte-code file \"%s\": %s",
	       filename, strerror (errno));
      return 0;
    }

  bc = js_bc_read_file (fp);
  fclose (fp);

  if (bc == NULL)
    /* XXX Error message. */
    return 0;

  /* Execute it. */

  result = js_vm_execute (interp->vm, bc);
  js_bc_free (bc);

  return result;
}


int
js_compile (JSInterpPtr interp,  char *input_file, char *assembler_file,
	    char *byte_code_file)
{
  JSNode argv[5];
  int i = 0;

  /* Init arguments. */

  argv[i].type = JS_INTEGER;
  argv[i].u.vinteger = 4;
  i++;

  /* Source file. */
  js_vm_make_static_string (interp->vm, &argv[i], input_file,
			    strlen (input_file));
  i++;

  /* Flags. */
  argv[i].type = JS_INTEGER;
  argv[i].u.vinteger = 0;

  if (interp->options.verbose)
    argv[i].u.vinteger |= JSC_FLAG_VERBOSE;
  if (interp->options.annotate_assembler)
    argv[i].u.vinteger |= JSC_FLAG_ANNOTATE_ASSEMBLER;
  if (interp->options.debug_info)
    argv[i].u.vinteger |= JSC_FLAG_GENERATE_DEBUG_INFO;

  if (interp->options.warn_unused_argument)
    argv[i].u.vinteger |= JSC_FLAG_WARN_UNUSED_ARGUMENT;
  if (interp->options.warn_unused_variable)
    argv[i].u.vinteger |= JSC_FLAG_WARN_UNUSED_VARIABLE;
  if (interp->options.warn_shadow)
    argv[i].u.vinteger |= JSC_FLAG_WARN_SHADOW;
  if (interp->options.warn_with_clobber)
    argv[i].u.vinteger |= JSC_FLAG_WARN_WITH_CLOBBER;

  if (interp->options.optimize_peephole)
    argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_PEEPHOLE;
  if (interp->options.optimize_jumps_to_jumps)
    argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_JUMPS;
  if (interp->options.optimize_bc_size)
    argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_BC_SIZE;
  if (interp->options.optimize_heavy)
    argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_HEAVY;

  i++;

  /* Assembler file. */
  if (assembler_file)
    js_vm_make_static_string (interp->vm, &argv[i], assembler_file,
			      strlen (assembler_file));
  else
    argv[i].type = JS_NULL;
  i++;

  /* Byte-code file. */
  if (byte_code_file)
    js_vm_make_static_string (interp->vm, &argv[i], byte_code_file,
			      strlen (byte_code_file));
  else
    argv[i].type = JS_NULL;
  i++;

  /* Call the compiler entry point. */
  return js_vm_apply (interp->vm, "JSC$compile_file", NULL, i, argv);
}


/* Type functions. */

void
js_type_make_string (JSInterpPtr interp, JSType *type, unsigned char *data,
		     unsigned int length)
{
  JSNode *n = (JSNode *) type;

  js_vm_make_string (interp->vm, n, data, length);
}


void
js_type_make_array (JSInterpPtr interp, JSType *type, unsigned int length)
{
  JSNode *n = (JSNode *) type;

  js_vm_make_array (interp->vm, n, length);
}


void
js_set_var (JSInterpPtr interp, char *name, JSType *value)
{
  JSNode *n = &interp->vm->globals[js_vm_intern (interp->vm, name)];
  JS_COPY (n, (JSNode *) value);
}


void
js_get_var (JSInterpPtr interp, char *name, JSType *value)
{
  JSNode *n = &interp->vm->globals[js_vm_intern (interp->vm, name)];
  JS_COPY ((JSNode *) value, n);
}


int
js_create_global_method (JSInterpPtr interp, char *name,
			 JSGlobalMethodProc proc, void *context,
			 JSFreeProc context_free_proc)
{
  JSNode *n = &interp->vm->globals[js_vm_intern (interp->vm, name)];
  JSVirtualMachine *vm = interp->vm;
  int result = 1;

  /* Need one toplevel here. */
  {
    JSErrorHandlerFrame handler;

    /* We must create the toplevel ourself. */
    memset (&handler, 0, sizeof (handler));
    handler.next = vm->error_handler;
    vm->error_handler = &handler;

    if (setjmp (vm->error_handler->error_jmp))
      /* An error occurred. */
      result = 0;
    else
      {
	JSBuiltinInfo *info;
	JSGlobalMethodContext *ctx;

	/* Context. */
	ctx = js_calloc (vm, 1, sizeof (*ctx));

	ctx->proc = proc;
	ctx->context = context;
	ctx->free_proc = context_free_proc;
	ctx->interp = interp;

	/* Info. */
	info = js_vm_builtin_info_create (vm);
	info->global_method_proc = js_global_method_stub;

	/* Create the builtin. */
	js_vm_builtin_create (interp->vm, n, info, ctx);
      }

    /* Pop the error handler. */
    vm->error_handler = vm->error_handler->next;
  }

  return result;
}


int
js_define_module (JSInterpPtr interp, JSModuleInitProc init_proc)
{
  JSErrorHandlerFrame handler;
  JSVirtualMachine *vm = interp->vm;
  int result = 1;

  /* Just call the init proc in a toplevel. */

  memset (&handler, 0, sizeof (handler));
  handler.next = vm->error_handler;
  vm->error_handler = &handler;

  if (setjmp (vm->error_handler->error_jmp))
    /* An error occurred. */
    result = 0;
  else
    /* Call the module init proc. */
    (*init_proc) (interp);

  /* Pop the error handler. */
  vm->error_handler = vm->error_handler->next;

  return result;
}



/*
 * Static functions.
 */
static int
js_eval_source (JSInterpPtr interp, JSNode *source, char *compiler_function)
{
  JSNode argv[5];
  int i = 0;
  int result;
  JSByteCode *bc;

  /* Let's compile the code. */

  /* Argument count. */
  argv[i].type = JS_INTEGER;
  argv[i].u.vinteger = 4;
  i++;

  /* Source to compiler. */
  JS_COPY (&argv[i], source);
  i++;

  /* Flags. */
  argv[i].type = JS_INTEGER;
  argv[i].u.vinteger = 0;

  if (interp->options.verbose)
    argv[i].u.vinteger = JSC_FLAG_VERBOSE;

  argv[i].u.vinteger |= JSC_FLAG_GENERATE_DEBUG_INFO;

  argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_PEEPHOLE;
  argv[i].u.vinteger |= JSC_FLAG_OPTIMIZE_JUMPS;
  argv[i].u.vinteger |= JSC_FLAG_WARN_MASK;
  i++;

  /* Assembler file. */
  argv[i].type = JS_NULL;
  i++;

  /* Byte-code file. */
  argv[i].type = JS_NULL;
  i++;

  /* Call the compiler entry point. */
  result = js_vm_apply (interp->vm, compiler_function, NULL, i, argv);
  if (result == 0)
    return 0;

  /*
   * The resulting byte-code file is now at vm->exec_result.
   *
   * Note!  The byte-code is a string allocated form the vm heap.
   * The garbage collector can free it when it wants since the result
   * isn't protected.  However, we have no risk here because we
   * first convert the byte-code data block to our internal
   * JSByteCode block that shares no memory with the original data.
   */

  assert (interp->vm->exec_result.type == JS_STRING);

  bc = js_bc_read_data (interp->vm->exec_result.u.vstring->data,
			interp->vm->exec_result.u.vstring->len);

  /* And finally, execute it. */
  result = js_vm_execute (interp->vm, bc);

  /* Free the byte-code. */
  js_bc_free (bc);

  return result;
}


/*
 * Static functions.
 */

static void
error_global_method (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
		     void *instance_context,
		     JSNode *result_return, JSNode *args)
{
  unsigned int len;

  if (args->u.vinteger != 1)
    {
      sprintf (vm->error, "eval: illegal amount of arguments");
      js_vm_error (vm);
    }
  if (args[1].type != JS_STRING)
    {
      sprintf (vm->error, "eval: illegal argument");
      js_vm_error (vm);
    }

  len = args[1].u.vstring->len;
  if (len > sizeof (vm->error) - 1)
    len = sizeof (vm->error) - 1;

  memcpy (vm->error, args[1].u.vstring->data, len);
  vm->error[len] = '\0';

  /* Here we go... */
  js_vm_error (vm);

  /* NOTREACHED */
}


static void
eval_global_method (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
		    void *instance_context, JSNode *result_return,
		    JSNode *args)
{
  JSInterpPtr interp = instance_context;

  if (args->u.vinteger != 1)
    {
      sprintf (vm->error, "eval: illegal amount of arguments");
      js_vm_error (vm);
    }
  if (args[1].type != JS_STRING)
    {
      sprintf (vm->error, "eval: illegal argument");
      js_vm_error (vm);
    }

  /*
   * Ok, we'r ready to eval it.  The source strings is our argument, so,
   * it is in the stack and therefore, protected for gc.
   */
  if (!js_eval_source (interp, &args[1], "JSC$compile_string"))
    {
      /* The evaluation failed. */
      fprintf (stderr, "eval: evaluation failed: %s\n", vm->error);
      result_return->type = JS_UNDEFINED;
    }
  else
    /* Pass the return value to our caller. */
    JS_COPY (result_return, &vm->exec_result);
}


static void
load_global_method (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
		    void *instance_context,
		    JSNode *result_return, JSNode *args)
{
  JSInterpPtr interp = instance_context;
  int i;
  int result;

  if (args->u.vinteger == 0)
    {
      sprintf (vm->error, "load: no arguments given");
      js_vm_error (vm);
    }

  for (i = 1; i <= args->u.vinteger; i++)
    {
      char *cp;

      if (args[i].type != JS_STRING)
	{
	  sprintf (vm->error, "load: illegal argument");
	  js_vm_error (vm);
	}

      cp = js_string_to_c_string (vm, &args[i]);
      result = js_eval_file (interp, cp);
      js_free (cp);

      if (!result)
	js_vm_error (vm);
    }

  result_return->type = JS_BOOLEAN;
  result_return->u.vboolean = 1;
}


static void
js_core_globals (JSInterpPtr interp)
{
  JSNode *n;
  JSBuiltinInfo *info;
  JSVirtualMachine *vm = interp->vm;

  /* Command `error'. */

  info = js_vm_builtin_info_create (vm);
  info->global_method_proc = error_global_method;

  n = &interp->vm->globals[js_vm_intern (interp->vm, "error")];
  js_vm_builtin_create (interp->vm, n, info, interp);

  if (!interp->options.no_compiler)
    {
      /* Command `eval'. */

      info = js_vm_builtin_info_create (vm);
      info->global_method_proc = eval_global_method;

      n = &interp->vm->globals[js_vm_intern (interp->vm, "eval")];

      js_vm_builtin_create (interp->vm, n, info, interp);
    }

  /* Command `load'. */

  info = js_vm_builtin_info_create (vm);
  info->global_method_proc = load_global_method;

  n = &interp->vm->globals[js_vm_intern (interp->vm, "load")];
  js_vm_builtin_create (interp->vm, n, info, interp);
}


static void
js_global_method_stub (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
		       void *instance_context, JSNode *result_return,
		       JSNode *args)
{
  JSMethodResult result;
  JSGlobalMethodContext *ctx = instance_context;

  /* Set the default result. */
  result_return->type = JS_UNDEFINED;

  /* Call the user supplied function. */
  result = (*ctx->proc) (ctx->context, ctx->interp, args->u.vinteger,
			 (JSType *) &args[1], (JSType *) result_return,
			 vm->error);
  if (result != JS_OK)
    js_vm_error (ctx->interp->vm);
}
