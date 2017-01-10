/*
 * The builtin File object.
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
 * $Source: /home/kde/kdesupport/js/b_file.c,v $
 * $Id: b_file.c,v 1.2.2.1 1999/07/15 15:02:49 porten Exp $
 */

/*
 * Class:
 *
 *   new File (PATH) => object
 *   File.byteToString (BYTE) => string
 * + File.lstat (PATH) => array / boolean
 * + File.remove (PATH) => boolean
 * + File.rename (FROM, TO) => boolean
 * + File.stat (PATH) => array / boolean
 *   File.stringToByte (STRING) => number
 *
 * Object:
 *
 *   fileObj.open (MODE) => boolean
 *   fileObj.close () => boolean
 *   fileObj.setPosition (POSITION [, WHENCE]) => boolean
 *   fileObj.getPosition () => integer
 *   fileObj.eof () => boolean
 *   fileObj.read (SIZE) => string
 *   fileObj.readln () => string
 *   fileObj.readByte () => integer
 *   fileObj.write (STRING) => boolean
 *   fileObj.writeln (STRING) => boolean
 *   fileObj.writeByte (INTEGER) => boolean
 * + fileObj.ungetByte (BYTE) => boolean
 *   fileObj.flush () => boolean
 *   fileObj.getLength () => integer
 *   fileObj.exists () => boolean
 *   fileObj.error () => integer
 *   fileObj.clearError () => true
 */

#include "jsint.h"

#include <sys/types.h>
#include <sys/stat.h>

/*
 * Types and definitions.
 */

/* Class context. */
struct file_ctx_st
{
  /* Static methods. */
  JSSymbol s_byteToString;
  JSSymbol s_lstat;
  JSSymbol s_remove;
  JSSymbol s_rename;
  JSSymbol s_stat;
  JSSymbol s_stringToByte;

  /* Methods */
  JSSymbol s_open;
  JSSymbol s_close;
  JSSymbol s_setPosition;
  JSSymbol s_getPosition;
  JSSymbol s_eof;
  JSSymbol s_read;
  JSSymbol s_readln;
  JSSymbol s_readByte;
  JSSymbol s_write;
  JSSymbol s_writeln;
  JSSymbol s_writeByte;
  JSSymbol s_ungetByte;
  JSSymbol s_flush;
  JSSymbol s_getLength;
  JSSymbol s_exists;
  JSSymbol s_error;
  JSSymbol s_clearError;
};

typedef struct file_ctx_st FileCtx;

/* Instance context. */
struct file_instance_ctx_st
{
  /* Flags. */
  unsigned int pipe : 1;
  unsigned int dont_close : 1;

  char *path;
  FILE *fp;
};

typedef struct file_instance_ctx_st FileInstanceCtx;


/*
 * Static functions.
 */

/* Method proc. */
static int
method (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
	void *instance_context, JSSymbol method, JSNode *result_return,
	JSNode *args)
{
  FileCtx *ctx = builtin_info->obj_context;
  FileInstanceCtx *ictx = instance_context;
  char buf[256];
  long int li = 0;
  int i = 0;

  /* The default result is false. */
  result_return->type = JS_BOOLEAN;
  result_return->u.vboolean = 0;

  /*
   * Static methods.
   */
  if (method == ctx->s_byteToString)
    {
      if (args->u.vinteger != 1)
	goto argument_error;

      i = -1;
      if (args[1].type == JS_INTEGER)
	{
	  i = args[1].u.vinteger;
	  if (i < 0 || i > 255)
	    i = -1;
	}

      js_vm_make_string (vm, result_return, NULL, 1);

      if (i < 0)
	result_return->u.vstring->len = 0;
      else
	result_return->u.vstring->data[0] = i;
    }
  /* ********************************************************************** */
  else if (method == ctx->s_lstat || method == ctx->s_stat)
    {
      char *path;
      struct stat stat_st;
      int result;

      if (args->u.vinteger != 1)
	goto argument_error;

      path = js_string_to_c_string (vm, &args[1]);

#if HAVE_LSTAT
      if (method == ctx->s_lstat)
	result = lstat (path, &stat_st);
      else
#endif /* HAVE_LSTAT */
	result = stat (path, &stat_st);

      js_free (path);

      if (result >= 0)
	{
	  JSNode *node;

	  /* Success. */
	  js_vm_make_array (vm, result_return, 13);
	  node = result_return->u.varray->data;

	  /* dev */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_dev;
	  node++;

	  /* ino */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_ino;
	  node++;

	  /* mode */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_mode;
	  node++;

	  /* nlink */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_nlink;
	  node++;

	  /* uid */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_uid;
	  node++;

	  /* gid */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_gid;
	  node++;

	  /* rdev */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_rdev;
	  node++;

	  /* size */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_size;
	  node++;

	  /* atime */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_atime;
	  node++;

	  /* mtime */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_mtime;
	  node++;

	  /* ctime */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_ctime;
	  node++;

	  /* blksize */
	  node->type = JS_INTEGER;
	  node->u.vinteger = stat_st.st_blksize;
	  node++;

	  /* blocks */
	  node->type = JS_INTEGER;
#if HAVE_STAT_ST_ST_BLOCKS
	  node->u.vinteger = stat_st.st_blocks;
#else /* not HAVE_STAT_ST_ST_BLOCKS */
	  node->u.vinteger = 0;
#endif /* not HAVE_STAT_ST_ST_BLOCKS */
	}
    }
  /* ********************************************************************** */
  else if (method == ctx->s_remove)
    {
      char *path;

      if (args->u.vinteger != 1)
	goto argument_error;

      if (args[1].type != JS_STRING)
	goto argument_type_error;

      path = js_string_to_c_string (vm, &args[1]);
      i = remove (path);
      js_free (path);

      result_return->u.vboolean = (i == 0);
    }
  /* ********************************************************************** */
  else if (method == ctx->s_rename)
    {
      char *path1;
      char *path2;

      if (args->u.vinteger != 2)
	goto argument_error;

      if (args[1].type != JS_STRING || args[2].type != JS_STRING)
	goto argument_type_error;

      path1 = js_string_to_c_string (vm, &args[1]);
      path2 = js_string_to_c_string (vm, &args[2]);

      i = rename (path1, path2);

      js_free (path1);
      js_free (path2);

      result_return->u.vboolean = (i == 0);
    }
  /* ********************************************************************** */
  else if (method == ctx->s_stringToByte)
    {
      if (args->u.vinteger != 1)
	goto argument_error;

      result_return->type = JS_INTEGER;

      if (args[1].type == JS_STRING && args[1].u.vstring->len > 0)
	result_return->u.vinteger = args[i].u.vstring->data[0];
      else
	result_return->u.vinteger = 0;
    }
  /* ********************************************************************** */
  else if (method == vm->syms.s_toString)
    {
      if (args->u.vinteger != 0)
	goto argument_error;

      if (ictx)
	js_vm_make_string (vm, result_return, ictx->path, strlen (ictx->path));
      else
	js_vm_make_static_string (vm, result_return, "File", 4);
    }
  /* ********************************************************************** */
  else if (ictx)
    {
      /*
       * Instance methods.
       */

      if (method == ctx->s_open)
	{
	  if (args->u.vinteger != 1)
	    goto argument_error;
	  if (args[1].type != JS_STRING || args[1].u.vstring->len > 10)
	    goto argument_type_error;

	  if (ictx->fp == NULL)
	    {
	      memcpy (buf, args[1].u.vstring->data, args[1].u.vstring->len);
	      buf[args[1].u.vstring->len] = 'b';
	      buf[args[1].u.vstring->len + 1] = '\0';

	      ictx->fp = fopen (ictx->path, buf);
	      if (ictx->fp != NULL)
		result_return->u.vboolean = 1;
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_close)
	{
	  if (ictx->fp != NULL)
	    {
	      int result;

	      if (ictx->pipe)
		result = pclose (ictx->fp);
	      else
		result = fclose (ictx->fp);

	      ictx->fp = NULL;
	      result_return->u.vboolean = result >= 0;
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_setPosition)
	{
	  if (args->u.vinteger == 1)
	    {
	      if (args[1].type != JS_INTEGER)
		goto argument_type_error;
	      li = args[1].u.vinteger;
	      i = SEEK_SET;
	    }
	  else if (args->u.vinteger == 2)
	    {
	      if (args[2].type == JS_INTEGER)
		{
		  switch (args[2].u.vinteger)
		    {
		    case 1:
		      i = SEEK_CUR;
		      break;

		    case 2:
		      i = SEEK_END;
		      break;

		    default:
		      i = SEEK_SET;
		      break;
		    }
		}
	      else
		i = SEEK_SET;
	    }
	  else
	    goto argument_error;

	  if (fseek (ictx->fp, li, i) >= 0)
	    result_return->u.vboolean = 1;
	}
      /* ***************************************************************** */
      else if (method == ctx->s_getPosition)
	{
	  result_return->type = JS_INTEGER;
	  if (ictx->fp == NULL)
	    result_return->u.vinteger = -1;
	  else
	    result_return->u.vinteger = ftell (ictx->fp);
	}
      /* ***************************************************************** */
      else if (method == ctx->s_eof)
	{
	  if (ictx->fp != NULL)
	    result_return->u.vboolean = feof (ictx->fp);
	}
      /* ***************************************************************** */
      else if (method == ctx->s_read)
	{
	  size_t got;
	  char *buffer;

	  if (args->u.vinteger != 1)
	    goto argument_error;
	  if (args[1].type != JS_INTEGER || args[1].u.vinteger < 0)
	    goto argument_type_error;

	  if (ictx->fp != NULL)
	    {
	      buffer = js_vm_alloc (vm, args[1].u.vinteger + 1);

	      got = fread (buffer, 1, args[1].u.vinteger, ictx->fp);
	      if (got == 0)
		js_vm_free (vm, buffer);
	      else
		{
		  js_vm_make_static_string (vm, result_return, buffer, got);
		  result_return->u.vstring->staticp = 0;
		}
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_readln)
	{
	  int ch;
	  unsigned int bufpos = 0;
	  unsigned int buflen = 0;
	  char *buffer = NULL;

	  if (args->u.vinteger != 0)
	    goto argument_error;

	  if (ictx->fp != NULL)
	    {
	      while ((ch = getc (ictx->fp)) != EOF && ch != '\n')
		{
		  if (bufpos >= buflen)
		    {
		      buflen += 1024;
		      buffer = js_vm_realloc (vm, buffer, buflen);
		    }
		  buffer[bufpos++] = ch;
		}

	      /* Remove '\r' characters. */
	      while (bufpos > 0)
		if (buffer[bufpos - 1] == '\r')
		  bufpos--;
		else
		  break;

	      js_vm_make_string (vm, result_return, buffer, bufpos);

	      if (buffer)
		js_vm_free (vm, buffer);
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_readByte)
	{
	  result_return->type = JS_INTEGER;
	  if (ictx->fp == NULL)
	    result_return->u.vinteger = -1;
	  else
	    result_return->u.vinteger = getc (ictx->fp);
	}
      /* ***************************************************************** */
      else if (method == ctx->s_write || method == ctx->s_writeln)
	{
	  size_t wrote;

	  if (args->u.vinteger != 1)
	    goto argument_error;
	  if (args[1].type != JS_STRING)
	    goto argument_type_error;

	  if (ictx->fp != NULL)
	    {
	      wrote = fwrite (args[1].u.vstring->data, 1,
			      args[1].u.vstring->len, ictx->fp);
	      if (wrote == args[1].u.vstring->len)
		{
		  /* Success. */
		  result_return->u.vboolean = 1;

		  if (method == ctx->s_writeln)
		    if (putc ('\n', ictx->fp) < 0)
		      /* No, it was not a success. */
		      result_return->u.vboolean = 0;
		}
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_writeByte)
	{
	  if (args->u.vinteger != 1)
	    goto argument_error;
	  if (args[1].type != JS_INTEGER)
	    goto argument_type_error;

	  if (ictx->fp != NULL)
	    result_return->u.vboolean
	      = putc (args[1].u.vinteger, ictx->fp) >= 0;
	}
      /* ***************************************************************** */
      else if (method == ctx->s_ungetByte)
	{
	  if (args->u.vinteger != 1)
	    goto argument_error;
	  if (args[1].type != JS_INTEGER)
	    goto argument_type_error;

	  if (ictx->fp != NULL)
	    result_return->u.vboolean
	      = ungetc (args[1].u.vinteger, ictx->fp) >= 0;
	}
      /* ***************************************************************** */
      else if (method == ctx->s_flush)
	{
	  if (args->u.vinteger != 0)
	    goto argument_error;

	  if (ictx->fp != NULL && fflush (ictx->fp) >= 0)
	    result_return->u.vboolean = 1;
	}
      /* ***************************************************************** */
      else if (method == ctx->s_getLength)
	{
	  long int cpos;

	  if (args->u.vinteger != 0)
	    goto argument_error;

	  /* The default error code is an integer -1. */
	  result_return->type = JS_INTEGER;
	  result_return->u.vinteger = -1;

	  if (ictx->fp != NULL)
	    {
	      /* Save current position. */
	      cpos = ftell (ictx->fp);
	      if (cpos >= 0)
		{
		  /* Seek to the end of the file. */
		  if (fseek (ictx->fp, 0, SEEK_END) >= 0)
		    {
		      /* Fetch result. */
		      result_return->u.vinteger = ftell (ictx->fp);

		      /* Seek back. */
		      if (fseek (ictx->fp, cpos, SEEK_SET) < 0)
			/* Couldn't revert the fp to the original position. */
			result_return->u.vinteger = -1;
		    }
		}
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_exists)
	{
	  if (args->u.vinteger != 0)
	    goto argument_error;

	  if (ictx->fp)
	    {
	      /* Since we have opened the file, it must exist. */
	      result_return->u.vboolean = 1;
	    }
	  else
	    {
	      struct stat stat_st;

	      if (stat (ictx->path, &stat_st) >= 0)
		result_return->u.vboolean = 1;
	    }
	}
      /* ***************************************************************** */
      else if (method == ctx->s_error)
	{
	  if (args->u.vinteger != 0)
	    goto argument_error;

	  result_return->type = JS_INTEGER;
	  if (ictx->fp == NULL)
	    result_return->u.vinteger = -1;
	  else
	    result_return->u.vinteger = ferror (ictx->fp);
	}
      /* ***************************************************************** */
      else if (method == ctx->s_clearError)
	{
	  if (args->u.vinteger != 0)
	    goto argument_error;

	  if (ictx->fp != NULL)
	    {
	      clearerr (ictx->fp);
	      result_return->u.vboolean = 1;
	    }
	}
      /* ***************************************************************** */
      else
	return JS_PROPERTY_UNKNOWN;
    }
  else
    return JS_PROPERTY_UNKNOWN;

  return JS_PROPERTY_FOUND;


  /*
   * Error handling.
   */

 argument_error:
  sprintf (vm->error, "File.%s(): illegal amount of arguments",
	   js_vm_symname (vm, method));
  js_vm_error (vm);

 argument_type_error:
  sprintf (vm->error, "File.%s(): illegal argument",
	   js_vm_symname (vm, method));
  js_vm_error (vm);

  /* NOTREACHED */
  return 0;
}

/* Property proc. */
static int
property (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info,
	  void *instance_context, JSSymbol property, int set, JSNode *node)
{
  /*  FileCtx *ctx = builtin_info->obj_context; */

  if (!set)
    node->type = JS_UNDEFINED;

  return JS_PROPERTY_UNKNOWN;
}


/* New proc. */
static void
new_proc (JSVirtualMachine *vm, JSBuiltinInfo *builtin_info, JSNode *args,
	  JSNode *result_return)
{
  FileInstanceCtx *instance;

  if (args->u.vinteger != 1)
    {
      sprintf (vm->error, "new File(): illegal amount of arguments");
      js_vm_error (vm);
    }
  if (args[1].type != JS_STRING)
    {
      sprintf (vm->error, "new File(): illegal argument");
      js_vm_error (vm);
    }

  instance = js_calloc (vm, 1, sizeof (*instance));
  instance->path = js_string_to_c_string (vm, &args[1]);

  js_vm_builtin_create (vm, result_return, builtin_info, instance);
}

/* Delete proc. */
static void
delete (JSBuiltinInfo *builtin_info, void *instance_context)
{
  FileInstanceCtx *ictx = instance_context;

  if (ictx)
    {
      if (ictx->fp)
	{
	  if (!ictx->dont_close)
	    {
	      if (ictx->pipe)
		pclose (ictx->fp);
	      else
		fclose (ictx->fp);
	    }
	  ictx->fp = NULL;
	}

      js_free (ictx->path);
      js_free (ictx);
    }
}

/*
 * Global functions.
 */

void
js_builtin_File (JSVirtualMachine *vm)
{
  JSNode *n;
  JSBuiltinInfo *info;
  FileCtx *ctx;

  ctx = js_calloc (vm, 1, sizeof (*ctx));

  ctx->s_byteToString	= js_vm_intern (vm, "byteToString");
  ctx->s_lstat		= js_vm_intern (vm, "lstat");
  ctx->s_remove		= js_vm_intern (vm, "remove");
  ctx->s_rename		= js_vm_intern (vm, "rename");
  ctx->s_stat		= js_vm_intern (vm, "stat");
  ctx->s_stringToByte	= js_vm_intern (vm, "stringToByte");

  ctx->s_open		= js_vm_intern (vm, "open");
  ctx->s_close		= js_vm_intern (vm, "close");
  ctx->s_setPosition	= js_vm_intern (vm, "setPosition");
  ctx->s_getPosition	= js_vm_intern (vm, "getPosition");
  ctx->s_eof		= js_vm_intern (vm, "eof");
  ctx->s_read		= js_vm_intern (vm, "read");
  ctx->s_readln		= js_vm_intern (vm, "readln");
  ctx->s_readByte	= js_vm_intern (vm, "readByte");
  ctx->s_write		= js_vm_intern (vm, "write");
  ctx->s_writeln	= js_vm_intern (vm, "writeln");
  ctx->s_writeByte	= js_vm_intern (vm, "writeByte");
  ctx->s_ungetByte	= js_vm_intern (vm, "ungetByte");
  ctx->s_flush		= js_vm_intern (vm, "flush");
  ctx->s_getLength	= js_vm_intern (vm, "getLength");
  ctx->s_exists		= js_vm_intern (vm, "exists");
  ctx->s_error		= js_vm_intern (vm, "error");
  ctx->s_clearError	= js_vm_intern (vm, "clearError");


  /* Object information. */

  info = js_vm_builtin_info_create (vm);

  info->method_proc 		= method;
  info->property_proc 		= property;
  info->new_proc 		= new_proc;
  info->delete_proc 		= delete;
  info->obj_context 		= ctx;
  info->obj_context_delete	= js_free;

  /* Define it. */
  n = &vm->globals[js_vm_intern (vm, "File")];
  js_vm_builtin_create (vm, n, info, NULL);
}


void
js_builtin_File_new (JSVirtualMachine *vm, JSNode *result_return,
		     char *path, FILE *fp, int pipe, int dont_close)
{
  JSNode *n;
  FileInstanceCtx *ictx;

  /* Lookup our context. */
  n = &vm->globals[js_vm_intern (vm, "File")];

  /* Create a file instance. */
  ictx = js_malloc (vm, sizeof (*ictx));
  ictx->path = js_strdup (vm, path);
  ictx->fp = fp;
  ictx->pipe = pipe;
  ictx->dont_close = dont_close;

  /* Create the builtin. */
  js_vm_builtin_create (vm, result_return, n->u.vbuiltin->info, ictx);
}
