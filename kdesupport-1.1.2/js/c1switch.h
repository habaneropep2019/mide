/* operand halt (0) */
case 0:
  SAVE_OP (0);
  cp += 0;
  break;

/* operand done (1) */
case 1:
  SAVE_OP (1);
  cp += 0;
  break;

/* operand nop (2) */
case 2:
  SAVE_OP (2);
  cp += 0;
  break;

/* operand dup (3) */
case 3:
  SAVE_OP (3);
  cp += 0;
  break;

/* operand pop (4) */
case 4:
  SAVE_OP (4);
  cp += 0;
  break;

/* operand pop_n (5) */
case 5:
  SAVE_OP (5);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand apop (6) */
case 6:
  SAVE_OP (6);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand swap (7) */
case 7:
  SAVE_OP (7);
  cp += 0;
  break;

/* operand roll (8) */
case 8:
  SAVE_OP (8);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand const (9) */
case 9:
  SAVE_OP (9);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand const_null (10) */
case 10:
  SAVE_OP (10);
  cp += 0;
  break;

/* operand const_true (11) */
case 11:
  SAVE_OP (11);
  cp += 0;
  break;

/* operand const_false (12) */
case 12:
  SAVE_OP (12);
  cp += 0;
  break;

/* operand const_undefined (13) */
case 13:
  SAVE_OP (13);
  cp += 0;
  break;

/* operand const_i0 (14) */
case 14:
  SAVE_OP (14);
  cp += 0;
  break;

/* operand const_i1 (15) */
case 15:
  SAVE_OP (15);
  cp += 0;
  break;

/* operand const_i2 (16) */
case 16:
  SAVE_OP (16);
  cp += 0;
  break;

/* operand const_i3 (17) */
case 17:
  SAVE_OP (17);
  cp += 0;
  break;

/* operand load_global (18) */
case 18:
  SAVE_OP (18);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand store_global (19) */
case 19:
  SAVE_OP (19);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand load_arg (20) */
case 20:
  SAVE_OP (20);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand store_arg (21) */
case 21:
  SAVE_OP (21);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand load_local (22) */
case 22:
  SAVE_OP (22);
  JS_BC_READ_INT16 (cp, i);
  SAVE_INT16 (i);
  cp += 2;
  break;

/* operand store_local (23) */
case 23:
  SAVE_OP (23);
  JS_BC_READ_INT16 (cp, i);
  SAVE_INT16 (i);
  cp += 2;
  break;

/* operand load_property (24) */
case 24:
  SAVE_OP (24);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand store_property (25) */
case 25:
  SAVE_OP (25);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand load_array (26) */
case 26:
  SAVE_OP (26);
  cp += 0;
  break;

/* operand store_array (27) */
case 27:
  SAVE_OP (27);
  cp += 0;
  break;

/* operand nth (28) */
case 28:
  SAVE_OP (28);
  cp += 0;
  break;

/* operand cmp_eq (29) */
case 29:
  SAVE_OP (29);
  cp += 0;
  break;

/* operand cmp_ne (30) */
case 30:
  SAVE_OP (30);
  cp += 0;
  break;

/* operand cmp_lt (31) */
case 31:
  SAVE_OP (31);
  cp += 0;
  break;

/* operand cmp_gt (32) */
case 32:
  SAVE_OP (32);
  cp += 0;
  break;

/* operand cmp_le (33) */
case 33:
  SAVE_OP (33);
  cp += 0;
  break;

/* operand cmp_ge (34) */
case 34:
  SAVE_OP (34);
  cp += 0;
  break;

/* operand sub (35) */
case 35:
  SAVE_OP (35);
  cp += 0;
  break;

/* operand add (36) */
case 36:
  SAVE_OP (36);
  cp += 0;
  break;

/* operand mul (37) */
case 37:
  SAVE_OP (37);
  cp += 0;
  break;

/* operand div (38) */
case 38:
  SAVE_OP (38);
  cp += 0;
  break;

/* operand mod (39) */
case 39:
  SAVE_OP (39);
  cp += 0;
  break;

/* operand neg (40) */
case 40:
  SAVE_OP (40);
  cp += 0;
  break;

/* operand and (41) */
case 41:
  SAVE_OP (41);
  cp += 0;
  break;

/* operand not (42) */
case 42:
  SAVE_OP (42);
  cp += 0;
  break;

/* operand or (43) */
case 43:
  SAVE_OP (43);
  cp += 0;
  break;

/* operand xor (44) */
case 44:
  SAVE_OP (44);
  cp += 0;
  break;

/* operand shift_left (45) */
case 45:
  SAVE_OP (45);
  cp += 0;
  break;

/* operand shift_right (46) */
case 46:
  SAVE_OP (46);
  cp += 0;
  break;

/* operand shift_rright (47) */
case 47:
  SAVE_OP (47);
  cp += 0;
  break;

/* operand iffalse (48) */
case 48:
  SAVE_OP (48);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand iftrue (49) */
case 49:
  SAVE_OP (49);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand call_method (50) */
case 50:
  SAVE_OP (50);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand jmp (51) */
case 51:
  SAVE_OP (51);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand jsr (52) */
case 52:
  SAVE_OP (52);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand return (53) */
case 53:
  SAVE_OP (53);
  cp += 0;
  break;

/* operand typeof (54) */
case 54:
  SAVE_OP (54);
  cp += 0;
  break;

/* operand new (55) */
case 55:
  SAVE_OP (55);
  cp += 0;
  break;

/* operand delete_property (56) */
case 56:
  SAVE_OP (56);
  JS_BC_READ_INT32 (cp, i);
  i += consts_offset;
  i = vm->consts[i].u.vsymbol;
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand delete_array (57) */
case 57:
  SAVE_OP (57);
  cp += 0;
  break;

/* operand locals (58) */
case 58:
  SAVE_OP (58);
  JS_BC_READ_INT16 (cp, i);
  SAVE_INT16 (i);
  cp += 2;
  break;

/* operand min_args (59) */
case 59:
  SAVE_OP (59);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand with_push (60) */
case 60:
  SAVE_OP (60);
  cp += 0;
  break;

/* operand with_pop (61) */
case 61:
  SAVE_OP (61);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand try_push (62) */
case 62:
  SAVE_OP (62);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand try_pop (63) */
case 63:
  SAVE_OP (63);
  JS_BC_READ_INT8 (cp, i);
  SAVE_INT8 (i);
  cp += 1;
  break;

/* operand throw (64) */
case 64:
  SAVE_OP (64);
  cp += 0;
  break;

/* operand iffalse_b (65) */
case 65:
  SAVE_OP (65);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand iftrue_b (66) */
case 66:
  SAVE_OP (66);
  JS_BC_READ_INT32 (cp, i);
  SAVE_INT32 (i);
  cp += 4;
  break;

/* operand add_1_i (67) */
case 67:
  SAVE_OP (67);
  cp += 0;
  break;

/* operand add_2_i (68) */
case 68:
  SAVE_OP (68);
  cp += 0;
  break;

