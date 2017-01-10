/* operand halt (0) */
op_halt:
  OPERAND (0);
  fprintf (stderr, "JavaScript VM: halt\n");
  while (1)
    sleep (5);
  NEXT ();

/* operand done (1) */
op_done:
  OPERAND (1);
  DONE ();
  NEXT ();

/* operand nop (2) */
op_nop:
  OPERAND (2);
  /* Nothing here! */
  NEXT ();

/* operand dup (3) */
op_dup:
  OPERAND (3);
  JS_COPY (JS_SP0, JS_SP1);
  JS_PUSH ();
  NEXT ();

/* operand pop (4) */
op_pop:
  OPERAND (4);
  JS_POP ();
  NEXT ();

/* operand pop_n (5) */
op_pop_n:
  OPERAND (5);
  READ_INT8 (i);
  sp += i;
  NEXT ();

/* operand apop (6) */
op_apop:
  OPERAND (6);
  READ_INT8 (i);
  JS_COPY (JS_SP (i + 1), JS_SP1);
  sp += i;
  NEXT ();

/* operand swap (7) */
op_swap:
  OPERAND (7);
  JS_COPY (JS_SP0, JS_SP2);
  JS_COPY (JS_SP2, JS_SP1);
  JS_COPY (JS_SP1, JS_SP0);
  NEXT ();

/* operand roll (8) */
op_roll:
  OPERAND (8);
  READ_INT8 (i8);

  if (i8 > 1)
    {
      int j;

      for (j = 0; j < i8; j++)
	JS_COPY (JS_SP (j), JS_SP (j + 1));

      JS_COPY (JS_SP (i8), JS_SP0);
    }
  else if (i8 < -1)
    {
      i8 = -i8;

      JS_COPY (JS_SP0, JS_SP (i8));
      for (; i8 > 0; i8--)
	JS_COPY (JS_SP (i8), JS_SP (i8 - 1));
    }
  NEXT ();

/* operand const (9) */
op_const:
  OPERAND (9);
  READ_INT32 (i);
  JS_COPY (JS_SP0, JS_CONST (i));
  JS_PUSH ();
  NEXT ();

/* operand const_null (10) */
op_const_null:
  OPERAND (10);
  JS_SP0->type = JS_NULL;
  JS_PUSH ();
  NEXT ();

/* operand const_true (11) */
op_const_true:
  OPERAND (11);
  JS_SP0->type = JS_BOOLEAN;
  JS_SP0->u.vboolean = 1;
  JS_PUSH ();
  NEXT ();

/* operand const_false (12) */
op_const_false:
  OPERAND (12);
  JS_SP0->type = JS_BOOLEAN;
  JS_SP0->u.vboolean = 0;
  JS_PUSH ();
  NEXT ();

/* operand const_undefined (13) */
op_const_undefined:
  OPERAND (13);
  JS_SP0->type = JS_UNDEFINED;
  JS_PUSH ();
  NEXT ();

/* operand const_i0 (14) */
op_const_i0:
  OPERAND (14);
  JS_SP0->type = JS_INTEGER;
  JS_SP0->u.vinteger = 0;
  JS_PUSH ();
  NEXT ();

/* operand const_i1 (15) */
op_const_i1:
  OPERAND (15);
  JS_SP0->type = JS_INTEGER;
  JS_SP0->u.vinteger = 1;
  JS_PUSH ();
  NEXT ();

/* operand const_i2 (16) */
op_const_i2:
  OPERAND (16);
  JS_SP0->type = JS_INTEGER;
  JS_SP0->u.vinteger = 2;
  JS_PUSH ();
  NEXT ();

/* operand const_i3 (17) */
op_const_i3:
  OPERAND (17);
  JS_SP0->type = JS_INTEGER;
  JS_SP0->u.vinteger = 3;
  JS_PUSH ();
  NEXT ();

/* operand load_global (18) */
op_load_global:
  OPERAND (18);
  READ_INT32 (j);

  {
    int found = 0;

    /* Loop over the with chain. */
    /* WITHCHAIN */
    if (JS_WITHPTR->u.iptr)
      {
	unsigned int *uip = JS_WITHPTR->u.iptr;
	unsigned int ui = *uip;
	JSNode *wp = (JSNode *) ((unsigned char *) uip
				 + sizeof (unsigned int));

	for (i = 0; i < ui; i++)
	  {
	    JSNode *w = &wp[i];
	    int result = JS_PROPERTY_UNKNOWN;

	    if (w->type == JS_BUILTIN)
	      {
		JS_SAVE_REGS ();
		if (w->u.vbuiltin->info->property_proc)
		  result = (*w->u.vbuiltin->info->property_proc) (
					vm,
					w->u.vbuiltin->info,
					w->u.vbuiltin->instance_context,
					j, 0, &builtin_result);
	      }
	    else if (w->type == JS_OBJECT)
	      {
		result = js_vm_object_load_property (vm, w->u.vobject, j,
						     &builtin_result);
	      }
	    else
	      ERROR ("corrupted with-chain in load_global");

	    if (result == JS_PROPERTY_FOUND)
	      {
		JS_COPY (JS_SP0, &builtin_result);
		JS_PUSH ();
		found = 1;
		break;
	      }
	  }
      }

    if (!found)
      {
	/* Use the global value. */
	JS_COPY (JS_SP0, JS_GLOBAL (j));
	JS_PUSH ();

	if (vm->warn_undef && JS_SP1->type == JS_UNDEFINED)
	  {
	    fprintf (stderr, "VM: warning: using undefined global `%s'\n",
		     js_vm_symname (vm, j));
	  }
      }
  }
  NEXT ();

/* operand store_global (19) */
op_store_global:
  OPERAND (19);
  READ_INT32 (i);

  /* Operand store_global do not check the with-chain. */
  /* WITHCHAIN */

  /* Set the global value. */
  JS_COPY (JS_GLOBAL (i), JS_SP1);
  JS_POP ();
  NEXT ();

/* operand load_arg (20) */
op_load_arg:
  OPERAND (20);
  READ_INT8 (i);
  JS_COPY (JS_SP0, JS_ARG (i));
  JS_PUSH ();
  NEXT ();

/* operand store_arg (21) */
op_store_arg:
  OPERAND (21);
  READ_INT8 (i);
  JS_COPY (JS_ARG (i), JS_SP1);
  JS_POP ();
  NEXT ();

/* operand load_local (22) */
op_load_local:
  OPERAND (22);
  READ_INT16 (i);
  JS_COPY (JS_SP0, JS_LOCAL (i));
  JS_PUSH ();
  NEXT ();

/* operand store_local (23) */
op_store_local:
  OPERAND (23);
  READ_INT16 (i);
  JS_COPY (JS_LOCAL (i), JS_SP1);
  JS_POP ();
  NEXT ();

/* operand load_property (24) */
op_load_property:
  OPERAND (24);
  /* Fetch the property symbol. */
  READ_INT32 (j);

  if (JS_SP1->type == JS_BUILTIN)
    {
      JS_SAVE_REGS ();
      if (JS_SP1->u.vbuiltin->info->property_proc)
	{
	  if ((*JS_SP1->u.vbuiltin->info->property_proc) (
					vm,
					JS_SP1->u.vbuiltin->info,
					JS_SP1->u.vbuiltin->instance_context,
					j, 0, &builtin_result)
	      == JS_PROPERTY_UNKNOWN)
	    {
	      if (j == vm->syms.s_prototype)
		{
		  /* Looking up the prototype. */

		  builtin_result.type = JS_OBJECT;
		  if (JS_SP1->u.vbuiltin->prototype)
		    /* This is an instance. */
		    builtin_result.u.vobject = JS_SP1->u.vbuiltin->prototype;
		  else
		    /* This is a class. */
		    builtin_result.u.vobject
		      = JS_SP1->u.vbuiltin->info->prototype;
		}
	      else
		{
		  /* Looking up stuffs from the prototype. */

		  if (JS_SP1->u.vbuiltin->prototype)
		    /* An instance. */
		    js_vm_object_load_property (vm,
						JS_SP1->u.vbuiltin->prototype,
						j, &builtin_result);
		  else
		    /* A class. */
		    js_vm_object_load_property (
					vm,
					JS_SP1->u.vbuiltin->info->prototype,
					j, &builtin_result);
		}
	    }
	  JS_COPY (JS_SP1, &builtin_result);
	}
      else
	ERROR ("illegal builtin object for load_property");
    }
  else if (JS_SP1->type == JS_OBJECT)
    {
      js_vm_object_load_property (vm, JS_SP1->u.vobject, j, JS_SP1);
    }
  else if (vm->prim[JS_SP1->type])
    {
      /* The primitive language types. */
      JS_SAVE_REGS ();
      if ((*vm->prim[JS_SP1->type]->property_proc) (vm, vm->prim[JS_SP1->type],
						    JS_SP1, j, 0,
						    &builtin_result)
	  == JS_PROPERTY_UNKNOWN)
	{
	  if (j == vm->syms.s_prototype)
	    {
	      /* Looking up the prototype. */
	      switch (JS_SP1->type)
		{
		case JS_STRING:
		  if (JS_SP1->u.vstring->prototype)
		    {
		      builtin_result.type = JS_OBJECT;
		      builtin_result.u.vobject = JS_SP1->u.vstring->prototype;
		    }
		  else
		    /* No prototype yet. */
		    builtin_result.type = JS_NULL;
		  break;

		case JS_ARRAY:
		  if (JS_SP1->u.varray->prototype)
		    {
		      builtin_result.type = JS_OBJECT;
		      builtin_result.u.vobject = JS_SP1->u.varray->prototype;
		    }
		  else
		    /* No prototype yet. */
		    builtin_result.type = JS_NULL;
		  break;

		case JS_FUNC:
		  if (JS_SP1->u.vfunction->prototype)
		    {
		      builtin_result.type = JS_OBJECT;
		      builtin_result.u.vobject
			= JS_SP1->u.vfunction->prototype;
		    }
		  else
		    /* No prototype yet. */
		    builtin_result.type = JS_NULL;
		  break;

		default:
		  /* The rest do not have prototype. */
		  builtin_result.type = JS_NULL;
		  break;
		}
	    }
	  else
	    {
	      /* Looking up stuffs from the prototype. */
	      switch (JS_SP1->type)
		{
		case JS_STRING:
		  if (JS_SP1->u.vstring->prototype)
		    js_vm_object_load_property (vm,
						JS_SP1->u.vstring->prototype,
						j, &builtin_result);
		  else
		    /* Take it from the class' prototype */
		    goto from_class_prototype;
		  break;

		case JS_ARRAY:
		  if (JS_SP1->u.varray->prototype)
		    js_vm_object_load_property (vm,
						JS_SP1->u.varray->prototype,
						j, &builtin_result);
		  else
		    /* Take it from the class' prototype */
		    goto from_class_prototype;
		  break;

		case JS_FUNC:
		  if (JS_SP1->u.vfunction->prototype)
		    js_vm_object_load_property (vm,
						JS_SP1->u.vfunction->prototype,
						j, &builtin_result);
		  else
		    /* Take it from the class' prototype */
		    goto from_class_prototype;
		  break;

		default:
		  /*
		   * The rest do not have instance prototypes; use the
		   * class prototypes.
		   */
		from_class_prototype:
		  js_vm_object_load_property (
					vm,
					vm->prim[JS_SP1->type]->prototype, j,
					&builtin_result);
		  break;
		}
	    }
	}

      JS_COPY (JS_SP1, &builtin_result);
    }
  else
    ERROR ("illegal object for load_property");
  NEXT ();

/* operand store_property (25) */
op_store_property:
  OPERAND (25);
  /* Fetch the property symbol. */
  READ_INT32 (j);

  if (JS_SP1->type == JS_BUILTIN)
    {
      JS_SAVE_REGS ();
      if (JS_SP1->u.vbuiltin->info->property_proc)
	{
	  if ((*JS_SP1->u.vbuiltin->info->property_proc) (
					vm,
					JS_SP1->u.vbuiltin->info,
					JS_SP1->u.vbuiltin->instance_context,
					j, 1, JS_SP2)
	      == JS_PROPERTY_UNKNOWN)
	    {
	      if (j == vm->syms.s_prototype)
		{
		  /* Setting the prototype. */
		  if (JS_SP2->type != JS_OBJECT)
		    ERROR ("illegal value for set_property");

		  if (JS_SP1->u.vbuiltin->prototype)
		    /* Setting the instance's prototype. */
		    JS_SP1->u.vbuiltin->prototype = JS_SP2->u.vobject;
		  else
		    /* Setting the class' prototype. */
		    JS_SP1->u.vbuiltin->info->prototype = JS_SP2->u.vobject;
		}
	      else
		{
		  /* Setting stuff to the prototype. */
		  if (JS_SP1->u.vbuiltin->prototype)
		    /* An instance. */
		    js_vm_object_store_property (vm,
						 JS_SP1->u.vbuiltin->prototype,
						 j, JS_SP2);
		  else
		    /* A class. */
		    js_vm_object_store_property (
					vm,
					JS_SP1->u.vbuiltin->info->prototype,
					j, JS_SP2);
		}
	    }
	}
      else
	ERROR ("illegal builtin object for store_property");

      JS_POP ();
      JS_POP ();
    }
  else if (JS_SP1->type == JS_OBJECT)
    {
      js_vm_object_store_property (vm, JS_SP1->u.vobject, j, JS_SP2);
      JS_POP ();
      JS_POP ();
    }
  else if (vm->prim[JS_SP1->type])
    {
      /* The primitive language types. */
      JS_SAVE_REGS ();
      if ((*vm->prim[JS_SP1->type]->property_proc) (vm, vm->prim[JS_SP1->type],
						    JS_SP1, j, 1, JS_SP2)
	  == JS_PROPERTY_UNKNOWN)
	{
	  if (j == vm->syms.s_prototype)
	    {
	      /* Setting the prototype. */
	      if (JS_SP2->type != JS_OBJECT)
		ERROR ("illegal value for set_property");

	      switch (JS_SP1->type)
		{
		case JS_STRING:
		  JS_SP1->u.vstring->prototype = JS_SP2->u.vobject;
		  break;

		case JS_ARRAY:
		  JS_SP1->u.varray->prototype = JS_SP2->u.vobject;
		  break;

		case JS_FUNC:
		  JS_SP1->u.vfunction->prototype = JS_SP2->u.vobject;
		  break;

		default:
		  ERROR ("illegal object for set_property");
		  break;
		}
	    }
	  else
	    {
	      JSNode prototype;

	      /* Setting to the prototype.  We create them on demand. */
	      switch (JS_SP1->type)
		{
		case JS_STRING:
		  if (JS_SP1->u.vstring->prototype == NULL)
		    {
		      prototype.type = JS_OBJECT;

		      /* Create the prototype and set its __proto__. */
		      JS_SP1->u.vstring->prototype = js_vm_object_new (vm);
		      prototype.u.vobject = vm->prim[JS_OBJECT]->prototype;
		      js_vm_object_store_property (
					vm,
					JS_SP1->u.vstring->prototype,
					vm->syms.s___proto__,
					&prototype);
		    }
		  js_vm_object_store_property (vm,
					       JS_SP1->u.vstring->prototype,
					       j, JS_SP2);
		  break;

		case JS_ARRAY:
		  if (JS_SP1->u.varray->prototype == NULL)
		    {
		      prototype.type = JS_OBJECT;

		      /* Create the prototype and set its __proto__. */
		      JS_SP1->u.varray->prototype = js_vm_object_new (vm);
		      prototype.u.vobject = vm->prim[JS_OBJECT]->prototype;
		      js_vm_object_store_property (
					vm,
					JS_SP1->u.varray->prototype,
					vm->syms.s___proto__,
					&prototype);
		    }
		  js_vm_object_store_property (vm,
					       JS_SP1->u.varray->prototype,
					       j, JS_SP2);
		  break;

		case JS_FUNC:
		  if (JS_SP1->u.vfunction->prototype == NULL)
		    {
		      prototype.type = JS_OBJECT;

		      /* Create the prototype and set its __proto__. */
		      JS_SP1->u.vfunction->prototype = js_vm_object_new (vm);
		      prototype.u.vobject = vm->prim[JS_OBJECT]->prototype;
		      js_vm_object_store_property (
					vm,
					JS_SP1->u.vfunction->prototype,
					vm->syms.s___proto__,
					&prototype);
		    }
		  js_vm_object_store_property (vm,
					       JS_SP1->u.vfunction->prototype,
					       j, JS_SP2);
		  break;

		default:
		  ERROR ("illegal object for set_property");
		  break;
		}
	    }
	}
      JS_POP ();
      JS_POP ();
    }
  else
    ERROR ("illegal object for store_property");

  JS_MAYBE_GC ();
  NEXT ();

/* operand load_array (26) */
op_load_array:
  OPERAND (26);
  if (JS_SP2->type == JS_BUILTIN)
    {
      ERROR ("load_array: JS_BUILTIN: not implemented yet");
    }
  else if (JS_SP2->type == JS_OBJECT)
    {
      js_vm_object_load_array (vm, JS_SP2->u.vobject, JS_SP1, JS_SP2);
      JS_POP ();
    }
  else if (JS_SP2->type == JS_ARRAY)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  if (JS_SP1->u.vinteger < 0
	      || JS_SP1->u.vinteger >= JS_SP2->u.varray->length)
	    JS_SP2->type = JS_UNDEFINED;
	  else
	    {
	      JSNode *n = &JS_SP2->u.varray->data[JS_SP1->u.vinteger];
	      JS_COPY (JS_SP2, n);
	    }
	  JS_POP ();
	}
      else
	ERROR ("illegal array index in load_array");
    }
  else if (JS_SP2->type == JS_STRING)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  int ch;

	  if (JS_SP1->u.vinteger < 0
	      || JS_SP1->u.vinteger >= JS_SP2->u.vstring->len)
	    ERROR ("string index out of range in load_array");

	  ch = JS_SP2->u.vstring->data[JS_SP1->u.vinteger];
	  JS_SP2->type = JS_INTEGER;
	  JS_SP2->u.vinteger = ch;

	  JS_POP ();
	}
      else
	ERROR ("illegal string index in load_array");
    }
  else
    ERROR ("illegal object for load_array");
  NEXT ();

/* operand store_array (27) */
op_store_array:
  OPERAND (27);
  if (JS_SP2->type == JS_BUILTIN)
    {
      ERROR ("store_array: JS_BUILTIN: not implemented yet");
    }
  else if (JS_SP2->type == JS_OBJECT)
    {
      js_vm_object_store_array (vm, JS_SP2->u.vobject, JS_SP1, JS_SP (3));
      sp += 3;
    }
  else if (JS_SP2->type == JS_ARRAY)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  if (JS_SP1->u.vinteger < 0)
	    ERROR ("negative array index in store_array");
	  if (JS_SP1->u.vinteger >= JS_SP2->u.varray->length)
	    js_vm_expand_array (vm, JS_SP2, JS_SP1->u.vinteger + 1);

	  JS_COPY (&JS_SP2->u.varray->data[JS_SP1->u.vinteger], JS_SP (3));
	  sp += 3;
	}
      else
	ERROR ("illegal array index in store_array");
    }
  else if (JS_SP2->type == JS_STRING)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  if (JS_SP2->u.vstring->staticp)
	    ERROR ("static string in store_array");

	  if (JS_SP1->u.vinteger < 0)
	    ERROR ("negative string index in store_array");

	  if (JS_SP (3)->type != JS_INTEGER)
	    ERROR ("non-integer value to store into string in store_array");

	  if (JS_SP1->u.vinteger >= JS_SP2->u.vstring->len)
	    {
	      /* Expand the string. */
	      JS_SP2->u.vstring->data = js_vm_realloc (vm,
						       JS_SP2->u.vstring->data,
						       JS_SP1->u.vinteger + 1);
	      /* Fill the gap with ' '. */
	      for (; JS_SP2->u.vstring->len <= JS_SP1->u.vinteger;)
		JS_SP2->u.vstring->data[JS_SP2->u.vstring->len++] = ' ';
	    }

	  JS_SP2->u.vstring->data[JS_SP1->u.vinteger]
	    = (unsigned char) JS_SP (3)->u.vinteger;
	  sp += 3;
	}
      else
	ERROR ("illegal string index in store_array");
    }
  else
    ERROR ("illegal object for store_array");

  JS_MAYBE_GC ();
  NEXT ();

/* operand nth (28) */
op_nth:
  OPERAND (28);
  if (JS_SP2->type == JS_STRING)
    {
      if (JS_SP1->u.vinteger < 0
	  || JS_SP1->u.vinteger >= JS_SP2->u.vstring->len)
	{
	  JS_SP2->type = JS_UNDEFINED;

	  JS_SP1->type = JS_BOOLEAN;
	  JS_SP1->u.vboolean = 0;
	}
      else
	{
	  JS_SP2->type = JS_INTEGER;
	  JS_SP2->u.vinteger = JS_SP2->u.vstring->data[JS_SP1->u.vinteger];

	  JS_SP1->type = JS_BOOLEAN;
	  JS_SP1->u.vboolean = 1;
	}
    }
  else if (JS_SP2->type == JS_ARRAY)
    {
      if (JS_SP1->u.vinteger < 0
	  || JS_SP1->u.vinteger >= JS_SP2->u.varray->length)
	{
	  JS_SP2->type = JS_UNDEFINED;

	  JS_SP1->type = JS_BOOLEAN;
	  JS_SP1->u.vboolean = 0;
	}
      else
	{
	  JS_COPY (JS_SP2, &JS_SP2->u.varray->data[JS_SP1->u.vinteger]);

	  JS_SP1->type = JS_BOOLEAN;
	  JS_SP1->u.vboolean = 1;
	}
    }
  else if (JS_SP2->type == JS_OBJECT)
    {
      i = js_vm_object_nth (vm, JS_SP2->u.vobject, JS_SP1->u.vinteger, JS_SP2);
      JS_SP1->type = JS_BOOLEAN;
      JS_SP1->u.vboolean = i;
    }
  else
    ERROR ("illegal object for nth");
  NEXT ();

/* operand cmp_eq (29) */
op_cmp_eq:
  OPERAND (29);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger == JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) == 0;
  else if (JS_SP2->type == JS_NULL && JS_SP1->type == JS_NULL)
    JS_SP2->u.vboolean = 1;
  else if (JS_SP2->type != JS_SP1->type)
    JS_SP2->u.vboolean = 0;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat == JS_SP1->u.vfloat;
  else if (JS_SP2->type == JS_BOOLEAN && JS_SP1->type == JS_BOOLEAN)
    JS_SP2->u.vboolean = JS_SP2->u.vboolean == JS_SP1->u.vboolean;
  else if (JS_SP2->type == JS_UNDEFINED && JS_SP1->type == JS_UNDEFINED)
    JS_SP2->u.vboolean = 1;
  else if (JS_SP2->type == JS_OBJECT && JS_SP1->type == JS_OBJECT)
    JS_SP2->u.vboolean = JS_SP2->u.vobject == JS_SP1->u.vobject;
  else if (JS_SP2->type == JS_SYMBOL && JS_SP1->type == JS_SYMBOL)
    JS_SP2->u.vboolean = JS_SP2->u.vsymbol == JS_SP1->u.vsymbol;
  else if (JS_SP2->type == JS_BUILTIN && JS_SP1->type == JS_BUILTIN)
    JS_SP2->u.vboolean = (JS_SP2->u.vbuiltin->info == JS_SP1->u.vbuiltin->info
			  && (JS_SP2->u.vbuiltin->instance_context
			      == JS_SP1->u.vbuiltin->instance_context));
  else if (JS_SP2->type == JS_FUNC && JS_SP1->type == JS_FUNC)
    JS_SP2->u.vboolean = JS_SP2->u.vfunction == JS_SP1->u.vfunction;
  else if (JS_SP2->type == JS_IPTR && JS_SP1->type == JS_IPTR)
    JS_SP2->u.vboolean = JS_SP2->u.iptr == JS_SP1->u.iptr;
  else
    JS_SP2->u.vboolean = 0;

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand cmp_ne (30) */
op_cmp_ne:
  OPERAND (30);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger != JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) != 0;
  else if (JS_SP2->type == JS_NULL && JS_SP1->type == JS_NULL)
    JS_SP2->u.vboolean = 0;
  else if (JS_SP2->type != JS_SP1->type)
    JS_SP2->u.vboolean = 1;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat != JS_SP1->u.vfloat;
  else if (JS_SP2->type == JS_BOOLEAN && JS_SP1->type == JS_BOOLEAN)
    JS_SP2->u.vboolean = JS_SP2->u.vboolean != JS_SP1->u.vboolean;
  else if (JS_SP2->type == JS_UNDEFINED && JS_SP1->type == JS_UNDEFINED)
    JS_SP2->u.vboolean = 0;
  else if (JS_SP2->type == JS_OBJECT && JS_SP1->type == JS_OBJECT)
    JS_SP2->u.vboolean = JS_SP2->u.vobject != JS_SP1->u.vobject;
  else if (JS_SP2->type == JS_SYMBOL && JS_SP1->type == JS_SYMBOL)
    JS_SP2->u.vboolean = JS_SP2->u.vsymbol != JS_SP1->u.vsymbol;
  else if (JS_SP2->type == JS_BUILTIN && JS_SP1->type == JS_BUILTIN)
    JS_SP2->u.vboolean = (JS_SP2->u.vbuiltin->info != JS_SP1->u.vbuiltin->info
			  || (JS_SP2->u.vbuiltin->instance_context
			      != JS_SP1->u.vbuiltin->instance_context));
  else if (JS_SP2->type == JS_FUNC && JS_SP1->type == JS_FUNC)
    JS_SP2->u.vboolean = JS_SP2->u.vfunction != JS_SP1->u.vfunction;
  else if (JS_SP2->type == JS_IPTR && JS_SP1->type == JS_IPTR)
    JS_SP2->u.vboolean = JS_SP2->u.iptr != JS_SP1->u.iptr;
  else
    JS_SP2->u.vboolean = 1;

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand cmp_lt (31) */
op_cmp_lt:
  OPERAND (31);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger < JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) < 0;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat < JS_SP1->u.vfloat;
  else
    ERROR ("illegal types for cmp_lt");

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand cmp_gt (32) */
op_cmp_gt:
  OPERAND (32);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger > JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) > 0;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat > JS_SP1->u.vfloat;
  else
    ERROR ("illegal types for cmp_gt");

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand cmp_le (33) */
op_cmp_le:
  OPERAND (33);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger <= JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) <= 0;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat <= JS_SP1->u.vfloat;
  else
    ERROR ("illegal types for cmp_le");

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand cmp_ge (34) */
op_cmp_ge:
  OPERAND (34);
  if (JS_SP2->type == JS_INTEGER && JS_SP1->type == JS_INTEGER)
    JS_SP2->u.vboolean = JS_SP2->u.vinteger >= JS_SP1->u.vinteger;
  else if (JS_SP2->type == JS_STRING && JS_SP1->type == JS_STRING)
    JS_SP2->u.vboolean = js_compare_strings (JS_SP2, JS_SP1) >= 0;
  else if (JS_SP2->type == JS_FLOAT && JS_SP1->type == JS_FLOAT)
    JS_SP2->u.vboolean = JS_SP2->u.vfloat >= JS_SP1->u.vfloat;
  else
    ERROR ("illegal types for cmp_ge");

  JS_SP2->type = JS_BOOLEAN;
  JS_POP ();
  NEXT ();

/* operand sub (35) */
op_sub:
  OPERAND (35);
#if TYPECHECK_TEST
  JS_SP2->u.vinteger -= JS_SP1->u.vinteger;
  JS_POP ();
#else /* not TYPECHECK_TEST */
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger -= JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->type = JS_FLOAT;
	  JS_SP2->u.vfloat = (double) JS_SP2->u.vinteger - JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for sub");
    }
  else if (JS_SP2->type == JS_FLOAT)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vfloat -= (double) JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->u.vfloat -= JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for sub");
    }
  else if (JS_SP2->type == JS_NAN)
    {
      JS_POP ();
    }
  else
    ERROR ("illegal types for sub");
#endif /* not TYPECHECK_TEST */
  NEXT ();

/* operand add (36) */
op_add:
  OPERAND (36);
#if TYPECHECK_TEST
  JS_SP2->u.vinteger += JS_SP1->u.vinteger;
  JS_POP ();
#else /* not TYPECHECK_TEST */
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger += JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->type = JS_FLOAT;
	  JS_SP2->u.vfloat = (double) JS_SP2->u.vinteger + JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for add");
    }
  else if (JS_SP2->type == JS_FLOAT)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vfloat += (double) JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->u.vfloat += JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for add");
    }
  else if (JS_SP2->type == JS_NAN)
    {
      JS_POP ();
    }
  else if (JS_SP2->type == JS_STRING)
    {
      if (JS_SP1->type == JS_STRING)
	{
	  unsigned char *ndata;
	  unsigned int nlen;

	  nlen = JS_SP2->u.vstring->len + JS_SP1->u.vstring->len;
	  ndata = js_vm_alloc (vm, nlen);
	  memcpy (ndata, JS_SP2->u.vstring->data, JS_SP2->u.vstring->len);
	  memcpy (ndata + JS_SP2->u.vstring->len, JS_SP1->u.vstring->data,
		  JS_SP1->u.vstring->len);

	  js_vm_make_static_string (vm, JS_SP2, ndata, nlen);
	  JS_SP2->u.vstring->staticp = 0;
	  JS_POP ();
	  JS_MAYBE_GC ();
	}
      else
	ERROR ("illegal types for add");
    }
  else
    ERROR ("illegal types for add");
#endif /* not TYPECHECK_TEST */
  NEXT ();

/* operand mul (37) */
op_mul:
  OPERAND (37);
#if TYPECHECK_TEST
  JS_SP2->u.vinteger *= JS_SP1->u.vinteger;
  JS_POP ();
#else /* not TYPECHECK_TEST */
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger *= JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->type = JS_FLOAT;
	  JS_SP2->u.vfloat = (double) JS_SP2->u.vinteger * JS_SP1->u.vfloat;
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for mul");
    }
  else if (JS_SP2->type == JS_FLOAT)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vfloat *= (double) JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->u.vfloat *= JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for mul");
    }
  else if (JS_SP2->type == JS_NAN)
    {
      JS_POP ();
    }
  else
    ERROR ("illegal types for mul");
#endif /* not TYPECHECK_TEST */
  NEXT ();

/* operand div (38) */
op_div:
  OPERAND (38);
#if TYPECHECK_TEST
  JS_SP2->u.vinteger /= JS_SP1->u.vinteger;
  JS_POP ();
#else /* not TYPECHECK_TEST */
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger /= JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->type = JS_FLOAT;
	  JS_SP2->u.vfloat = (double) JS_SP2->u.vinteger / JS_SP1->u.vfloat;
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for div");
    }
  else if (JS_SP2->type == JS_FLOAT)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vfloat /= (double) JS_SP1->u.vinteger;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_FLOAT)
	{
	  JS_SP2->u.vfloat /= JS_SP1->u.vfloat;
	  JS_POP ();
	}
      else if (JS_SP1->type == JS_NAN)
	{
	  JS_SP2->type = JS_NAN;
	  JS_POP ();
	}
      else
	ERROR ("illegal types for div");
    }
  else if (JS_SP2->type == JS_NAN)
    {
      JS_POP ();
    }
  else
    ERROR ("illegal types for div");
#endif /* not TYPECHECK_TEST */
  NEXT ();

/* operand mod (39) */
op_mod:
  OPERAND (39);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger % JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for mod");
    }
  else
    ERROR ("illegal types for mod");
  NEXT ();

/* operand neg (40) */
op_neg:
  OPERAND (40);
  if (JS_SP1->type == JS_INTEGER)
    JS_SP1->u.vinteger = -JS_SP1->u.vinteger;
  else if (JS_SP1->type == JS_FLOAT)
    JS_SP1->u.vfloat = -JS_SP1->u.vfloat;
  else
    ERROR ("illegal type for neg");
  NEXT ();

/* operand and (41) */
op_and:
  OPERAND (41);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger & JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for and");
    }
  else
    ERROR ("illegal types for and");
  NEXT ();

/* operand not (42) */
op_not:
  OPERAND (42);
  JS_SP1->u.vboolean = JS_IS_FALSE (JS_SP1);
  JS_SP1->type = JS_BOOLEAN;
  NEXT ();

/* operand or (43) */
op_or:
  OPERAND (43);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger | JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for or");
    }
  else
    ERROR ("illegal types for or");
  NEXT ();

/* operand xor (44) */
op_xor:
  OPERAND (44);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger ^ JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for xor");
    }
  else
    ERROR ("illegal types for xor");
  NEXT ();

/* operand shift_left (45) */
op_shift_left:
  OPERAND (45);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger << JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for shift_left");
    }
  else
    ERROR ("illegal types for shift_left");
  NEXT ();

/* operand shift_right (46) */
op_shift_right:
  OPERAND (46);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger >> JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for shift_right");
    }
  else
    ERROR ("illegal types for shift_right");
  NEXT ();

/* operand shift_rright (47) */
op_shift_rright:
  OPERAND (47);
  if (JS_SP2->type == JS_INTEGER)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  ERROR ("shift_rright not implemented yet");
	  JS_SP2->u.vinteger = (JS_SP2->u.vinteger >> JS_SP1->u.vinteger);
	  JS_POP ();
	}
      else
	ERROR ("illegal types for shift_rright");
    }
  else
    ERROR ("illegal types for shift_rright");
  NEXT ();

/* operand iffalse (48) */
op_iffalse:
  OPERAND (48);
  READ_INT32 (i);
  if (JS_IS_FALSE (JS_SP1))
    SETPC_RELATIVE (i);
  JS_POP ();
  NEXT ();

/* operand iftrue (49) */
op_iftrue:
  OPERAND (49);
  READ_INT32 (i);
  if (JS_IS_TRUE (JS_SP1))
    SETPC_RELATIVE (i);
  JS_POP ();
  NEXT ();

/* operand call_method (50) */
op_call_method:
  OPERAND (50);
  /* Fetch the method symbol. */
  READ_INT32 (j);

  if (JS_SP1->type == JS_BUILTIN)
    {
      JS_SAVE_REGS ();
      if (JS_SP1->u.vbuiltin->info->method_proc)
	{
	  if ((*JS_SP1->u.vbuiltin->info->method_proc) (
				vm,
				JS_SP1->u.vbuiltin->info,
				JS_SP1->u.vbuiltin->instance_context, j,
				&builtin_result, JS_SP2)
	      == JS_PROPERTY_UNKNOWN)
	    ERROR ("call_method: unknown method");
	}
      else
	ERROR ("illegal builtin object for call_method");

      JS_COPY (JS_SP0, &builtin_result);
      JS_PUSH ();
      JS_MAYBE_GC ();
    }
  else if (JS_SP1->type == JS_OBJECT)
    {
      JSNode method;

      js_vm_object_load_property (vm, JS_SP1->u.vobject, j, &method);
      if (method.type != JS_FUNC)
	ERROR ("call_method: unknown method");

      /* And once again.  We must do a subroutine call here. */
      JS_SUBROUTINE_CALL (method.u.vfunction->implementation);
    }
  else if (vm->prim[JS_SP1->type])
    {
      /* The primitive language types. */
      JS_SAVE_REGS ();
      if ((*vm->prim[JS_SP1->type]->method_proc) (vm, vm->prim[JS_SP1->type],
						  JS_SP1, j, &builtin_result,
						  JS_SP2)
	  == JS_PROPERTY_UNKNOWN)
	ERROR ("call_method: unknown method");

      JS_COPY (JS_SP0, &builtin_result);
      JS_PUSH ();
    }
  else
    ERROR ("illegal object for call_method");
  NEXT ();

/* operand jmp (51) */
op_jmp:
  OPERAND (51);
  READ_INT32 (i);
  SETPC_RELATIVE (i);
  NEXT ();

/* operand jsr (52) */
op_jsr:
  OPERAND (52);
  /* Read the subroutine symbol index. */
  READ_INT32 (j);

  {
    int found = 0;

    /* Loop over the with-chain. */
    /* WITHCHAIN */
    if (JS_WITHPTR->u.iptr)
      {
	unsigned int *uip = JS_WITHPTR->u.iptr;
	unsigned int ui = *uip;
	JSNode *wp = (JSNode *) ((unsigned char *) uip
				 + sizeof (unsigned int));

	for (i = 0; i < ui; i++)
	  {
	    JSNode *w = &wp[i];
	    int result = JS_PROPERTY_UNKNOWN;

	    if (w->type == JS_BUILTIN)
	      {
		JS_SAVE_REGS ();
		if (w->u.vbuiltin->info->method_proc)
		  result = (*w->u.vbuiltin->info->method_proc) (
					vm,
					w->u.vbuiltin->info,
					w->u.vbuiltin->instance_context, j,
					&builtin_result, JS_SP2);
		JS_MAYBE_GC ();

		if (result == JS_PROPERTY_FOUND)
		  {
		    JS_COPY (JS_SP0, &builtin_result);
		    JS_PUSH ();
		  }
	      }
	    else if (w->type == JS_OBJECT)
	      {
		JSNode method;

		js_vm_object_load_property (vm, w->u.vobject, j, &method);
		if (method.type == JS_FUNC)
		  {
		    result = JS_PROPERTY_FOUND;

		    /* Hopefully for the last time: do a subroutine call. */

		    /* First: replace the null `this' with `w'. */
		    JS_COPY (JS_SP1, w);

		    /* Then, do the normal subroutine call. */
		    JS_SUBROUTINE_CALL (method.u.vfunction->implementation);
		  }
	      }
	    else
	      ERROR ("corrupted with-chain in jsr");

	    if (result == JS_PROPERTY_FOUND)
	      {
		found = 1;
		break;
	      }
	  }
      }

    if (!found)
      {
	/* Call the global method. */
	function = JS_GLOBAL (j);

	if (function->type == JS_BUILTIN
	    && function->u.vbuiltin->info->global_method_proc)
	  {
	    JS_SAVE_REGS ();
	    (*function->u.vbuiltin->info->global_method_proc) (
				vm,
				function->u.vbuiltin->info,
				function->u.vbuiltin->instance_context,
				&builtin_result,
				JS_SP2);

	    JS_COPY (JS_SP0, &builtin_result);
	    JS_PUSH ();
	  }
	else if (function->type == JS_FUNC)
	  {
	    JS_SUBROUTINE_CALL (function->u.vfunction->implementation);
	  }
	else
	  {
	    fprintf (stderr, "symbol \"%s\" is undefined as function\n",
		     js_vm_symname (vm, j));
	    ERROR ("symbol's value as function is undefined");
	  }
      }
  }
  NEXT ();

/* operand return (53) */
op_return:
  OPERAND (53);
  if (fp->u.iptr == NULL)
    /* Return from the global scope. */
    DONE ();

  /* STACKFRAME */

  /* Set pc to the saved return address. */
#if 0
  if (fp[-2].type != JS_IPTR)
    ERROR ("can't find saved return address");
#endif
  pc = fp[-2].u.iptr;

  {
    void *old_fp;

    /* Save old frame pointer. */
#if 0
    if (fp->type != JS_IPTR)
      ERROR ("can't find saved frame pointer");
#endif
    old_fp = fp->u.iptr;

    /* Put return value to its correct location. */
    JS_COPY (fp, JS_SP1);

    /* Restore sp. */
    sp = &fp[-1];

    /* Restore frame pointer. */
    fp = old_fp;
  }
  NEXT ();

/* operand typeof (54) */
op_typeof:
  OPERAND (54);
  {
    char *typeof_name = "";	/* Initialized to make compiler quiet. */

    switch (JS_SP1->type)
      {
      case JS_UNDEFINED:
	typeof_name = "undefined";
	break;

      case JS_NULL:
	typeof_name = "object";
	break;

      case JS_BOOLEAN:
	typeof_name = "boolean";
	break;

      case JS_STRING:
	typeof_name = "string";
	break;

      case JS_INTEGER:
      case JS_FLOAT:
      case JS_NAN:
	typeof_name = "number";
	break;

      case JS_SYMBOL:
	typeof_name = "#symbol";
	break;

      case JS_BUILTIN:
	typeof_name = "XXX builtin";
	break;

      case JS_IPTR:
	typeof_name = "#iptr";
	break;

      case JS_OBJECT:
	typeof_name = "#object";
	break;

      case JS_ARRAY:
	typeof_name = "#array";
	break;

      case JS_FUNC:
	typeof_name = "#function";
	break;
      }

    js_vm_make_static_string (vm, JS_SP1, typeof_name, strlen (typeof_name));
    JS_MAYBE_GC ();
  }
  NEXT ();

/* operand new (55) */
op_new:
  OPERAND (55);
  /* Check object. */
  if (JS_SP1->type == JS_BUILTIN && JS_SP1->u.vbuiltin->info->new_proc)
    {
      JS_SAVE_REGS ();
      (*JS_SP1->u.vbuiltin->info->new_proc) (vm, JS_SP1->u.vbuiltin->info,
					     JS_SP2, JS_SP1);

      /* Push a dummy return value for the constructor.  This is ignored. */
      JS_SP0->type = JS_UNDEFINED;
      JS_PUSH ();
    }
  else if (JS_SP1->type == JS_FUNC)
    {
      JSObject *obj;
      JSNode f;
      JSNode prototype;

      /* The prototype is an object. */
      prototype.type = JS_OBJECT;

      /* Create the prototype for the function, if it is not defined. */
      if (JS_SP1->u.vfunction->prototype == NULL)
	{
	  JS_SP1->u.vfunction->prototype = js_vm_object_new (vm);

	  /* Set its __proto__ to point to Object's prototype.  */
	  prototype.u.vobject = vm->prim[JS_OBJECT]->prototype;
	  js_vm_object_store_property (vm, JS_SP1->u.vfunction->prototype,
				       vm->syms.s___proto__, &prototype);
	}

      /* Allocate a new object and set its prototype. */

      obj = js_vm_object_new (vm);

      prototype.u.vobject = JS_SP1->u.vfunction->prototype;
      js_vm_object_store_property (vm, obj, vm->syms.s___proto__, &prototype);

      /*
       * Basicly we do a jsr to the function given in JS_SP1.  But first,
       * we must set `this' pointer to the correct value.  See `jsr' for
       * the details.
       */
      JS_COPY (&f, JS_SP1);

      /* Replace func with the new object. */
      JS_SP1->type = JS_OBJECT;
      JS_SP1->u.vobject = obj;

      JS_SUBROUTINE_CALL (f.u.vfunction->implementation);
    }
  /* The primitive language types. */
  else if (vm->prim[JS_SP1->type])
    {
      JS_SAVE_REGS ();
      (*vm->prim[JS_SP1->type]->new_proc) (vm, vm->prim[JS_SP1->type], JS_SP2,
					   JS_SP1);
      JS_PUSH ();
    }
  else
    ERROR ("illegal object for new");

  JS_MAYBE_GC ();
  NEXT ();

/* operand delete_property (56) */
op_delete_property:
  OPERAND (56);
  /* Fetch the property symbol. */
  READ_INT32 (j);

  if (JS_SP1->type == JS_BUILTIN)
    {
      /*
       * XXX It should be possible to apply delete operand to builtin
       * XXX objects.
       */
      ERROR ("delete_property: JS_BUILTIN: not implemented yet");
    }
  else if (JS_SP1->type == JS_OBJECT)
    {
      js_vm_object_delete_property (vm, JS_SP1->u.vobject, j);
    }
  else if (JS_SP1->type == JS_NULL)
    {
      /* Delete a property from an object in the with-chain. */
      /* WITHCHAIN */
      ERROR ("delete_property: not implemented yet for the with-chain objects");
    }
  /* The primitive language types. */
  /*
   * XXX Since we can't delete properties from builtins, we can't delete
   * XXX them from the primitive language types.
   */
  else
    ERROR ("illegal object for delete_property");

  /* The delete operand returns an undefined value. */
  JS_SP1->type = JS_UNDEFINED;
  NEXT ();

/* operand delete_array (57) */
op_delete_array:
  OPERAND (57);
  if (JS_SP2->type == JS_BUILTIN)
    {
      ERROR ("delete_array: JS_BUILTIN: not implemented yet");
    }
  else if (JS_SP2->type == JS_OBJECT)
    {
      js_vm_object_delete_array (vm, JS_SP2->u.vobject, JS_SP1);
      JS_POP ();
    }
  else if (JS_SP2->type == JS_ARRAY)
    {
      if (JS_SP1->type == JS_INTEGER)
	{
	  if (0 <= JS_SP1->u.vinteger
	      && JS_SP1->u.vinteger < JS_SP2->u.varray->length)
	    JS_SP2->u.varray->data[JS_SP1->u.vinteger].type = JS_UNDEFINED;
	  JS_POP ();
	}
      else
	ERROR ("illegal array index in delete_array");
    }
  else
    ERROR ("illegal object for delete_array");

  /* The delete operand returns an undefined value. */
  JS_SP1->type = JS_UNDEFINED;
  NEXT ();

/* operand locals (58) */
op_locals:
  OPERAND (58);
  READ_INT16 (i);
  if (sp - i - 10 < vm->stack)
    ERROR ("stack overflow");

  for (; i > 0; i--)
    {
      JS_SP0->type = JS_UNDEFINED;
      JS_PUSH ();
    }
  NEXT ();

/* operand min_args (59) */
op_min_args:
  OPERAND (59);
  READ_INT8 (i);

  if (JS_SP1->u.vinteger < i)
    {
      unsigned int delta = i - JS_SP1->u.vinteger;
      unsigned int argc = JS_SP1->u.vinteger;

      memmove (JS_SP1 - delta, JS_SP1,
	       (fp - JS_SP0 + argc) * sizeof (JSNode));
      sp -= delta;
      fp -= delta;

      for (; argc < i; argc++)
	JS_ARG (argc)->type = JS_UNDEFINED;
    }

  JS_POP ();
  NEXT ();

/* operand with_push (60) */
op_with_push:
  OPERAND (60);
  if (JS_SP1->type != JS_OBJECT && JS_SP1->type != JS_BUILTIN)
    ERROR ("illegal object for with_push");

  if (JS_WITHPTR->u.iptr == NULL)
    {
      JSNode *np;
      unsigned int *ip = js_vm_alloc (vm,
				      sizeof (unsigned int)
				      + sizeof (JSNode));
      *ip = 1;
      np = (JSNode *) ((unsigned char *) ip + sizeof (unsigned int));

      JS_COPY (np, JS_SP1);
      JS_WITHPTR->u.iptr = ip;
    }
  else
    {
      JSNode *np;
      unsigned int *ip = JS_WITHPTR->u.iptr;
      unsigned int ui = *ip;

      ip = js_vm_realloc (vm, ip,
			  sizeof (unsigned int)
			  + ((ui + 1) * sizeof (JSNode)));
      (*ip)++;
      np = (JSNode *) ((unsigned char *) ip + sizeof (unsigned int));

      JS_COPY (&np[ui], JS_SP1);
      JS_WITHPTR->u.iptr = ip;
    }

  JS_POP ();
  NEXT ();

/* operand with_pop (61) */
op_with_pop:
  OPERAND (61);
  READ_INT8 (i);

  {
    unsigned int *ip = JS_WITHPTR->u.iptr;

    if (ip == NULL || *ip < i)
      ERROR ("with stack underflow in with_pop");

    *ip -= i;
  }
  NEXT ();

/* operand try_push (62) */
op_try_push:
  OPERAND (62);
  READ_INT32 (i);

  {
    JSErrorHandlerFrame *frame = js_calloc (vm, 1, sizeof (*frame));

    frame->next = vm->error_handler;
    frame->sp = sp;
    frame->fp = fp;
    frame->pc = pc;
    frame->pc_delta = i;
    vm->error_handler = frame;

    if (setjmp (vm->error_handler->error_jmp))
      {
	/* Ok, we caught an error. */

	/* Restore our state. */
	sp = vm->error_handler->sp;
	fp = vm->error_handler->fp;
	pc = vm->error_handler->pc;
	i = vm->error_handler->pc_delta;

	/* Push the thrown valus to the stack. */
	JS_COPY (JS_SP0, &vm->error_handler->thrown);
	JS_PUSH ();

	/* Remove this handler frame. */
	frame = vm->error_handler;
	vm->error_handler = vm->error_handler->next;
	js_free (frame);

	/* Do the jump to the catch block. */
	SETPC_RELATIVE (i);
      }
  }
  NEXT ();

/* operand try_pop (63) */
op_try_pop:
  OPERAND (63);
  READ_INT8 (i);

  for (; i > 0; i--)
    {
      JSErrorHandlerFrame *frame = vm->error_handler;

      vm->error_handler = vm->error_handler->next;
      js_free (frame);
    }
  NEXT ();

/* operand throw (64) */
op_throw:
  OPERAND (64);
  {
    JSErrorHandlerFrame *f = vm->error_handler;

    if (f->sp == NULL)
      /* We are jumping to the C-toplevel.  This is an uncaught exception. */
      ERROR ("uncaught exception");

    JS_COPY (&f->thrown, JS_SP1);
    longjmp (f->error_jmp, 1);

    /* NOTREACHED (I hope). */
    fprintf (stderr, "VM: no valid error handler initialized\n");
    abort ();
  }
  NEXT ();

/* operand iffalse_b (65) */
op_iffalse_b:
  OPERAND (65);
  READ_INT32 (i);
  if (!JS_SP1->u.vboolean)
    SETPC_RELATIVE (i);
  JS_POP ();
  NEXT ();

/* operand iftrue_b (66) */
op_iftrue_b:
  OPERAND (66);
  READ_INT32 (i);
  if (JS_SP1->u.vboolean)
    SETPC_RELATIVE (i);
  JS_POP ();
  NEXT ();

/* operand add_1_i (67) */
op_add_1_i:
  OPERAND (67);
  JS_SP1->u.vinteger++;
  NEXT ();

/* operand add_2_i (68) */
op_add_2_i:
  OPERAND (68);
  JS_SP1->u.vinteger += 2;
  NEXT ();

