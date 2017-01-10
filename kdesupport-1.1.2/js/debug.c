/*
 * Debugging utilities.
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
 * $Source: /home/kde/kdesupport/js/debug.c,v $
 * $Id: debug.c,v 1.1.1.1 1998/08/09 21:05:33 kulow Exp $
 */

#include "jsint.h"

/*
 * Global functions.
 */

void
js_vm_stacktrace (JSVirtualMachine *vm, unsigned int num_frames)
{
  unsigned int frame = 0;
  JSNode *sp = vm->sp;
  void *pc = vm->pc;
  JSNode *fp;

  fprintf (stderr,
	   "VM: stacktrace: stacksize=%d, used=%d\n",
	   vm->stack_size,
	   (vm->stack + vm->stack_size - sp));

  /* STACKFRAME */

  /* Find frame pointer. */
  for (fp = sp + 1; fp->type != JS_IPTR; fp++)
    ;

  /* The first iptr is return address. */
  fp++;

  /* The second iptr is the with pointer.  The third iptr is the fp. */
  fp++;

  while (fp && frame < num_frames)
    {
      JSNode *n;
      const char *func_name = js_vm_func_name (vm, pc);

      fprintf (stderr, "#%-3u %s%s:", frame++, func_name,
	       func_name[0] == '.' ? "" : "()");
      for (n = sp + 1; n != fp - 2; n++)
	{
	  switch (n->type)
	    {
	    case JS_UNDEFINED:
	      fprintf (stderr, " undefined");
	      break;

	    case JS_NULL:
	      fprintf (stderr, " null");
	      break;

	    case JS_BOOLEAN:
	      fprintf (stderr, " %s", n->u.vboolean ? "true" : "false");
	      break;

	    case JS_INTEGER:
	      fprintf (stderr, " %ld", n->u.vinteger);
	      break;

	    case JS_STRING:
	      fprintf (stderr, " \"%.*s\"",
		       (int) n->u.vstring->len,
		       n->u.vstring->data);
	      break;

	    case JS_FLOAT:
	      fprintf (stderr, " %g", n->u.vfloat);
	      break;

	    case JS_ARRAY:
	      fprintf (stderr, " array");
	      break;

	    case JS_OBJECT:
	      fprintf (stderr, " object");
	      break;

	    case JS_SYMBOL:
	      fprintf (stderr, " %s", js_vm_symname (vm, n->u.vsymbol));
	      break;

	    case JS_BUILTIN:
	      fprintf (stderr, " builtin");
	      break;

	    case JS_FUNC:
	      fprintf (stderr, " function");
	      break;

	    case JS_IPTR:
	      fprintf (stderr, " 0x%lx", (unsigned long) n->u.iptr);
	      break;

	    default:
	      fprintf (stderr, " XXX-please-update-type-%d", n->type);
	      break;
	    }
	}
      fprintf (stderr, "\n");

      sp = fp;
      pc = fp[-2].u.iptr;
      fp = fp->u.iptr;
    }
}
