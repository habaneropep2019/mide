/* operand halt (0) */
case 0:
  cpos++;
  break;

/* operand done (1) */
case 1:
  cpos++;
  break;

/* operand nop (2) */
case 2:
  cpos++;
  break;

/* operand dup (3) */
case 3:
  cpos++;
  break;

/* operand pop (4) */
case 4:
  cpos++;
  break;

/* operand pop_n (5) */
case 5:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand apop (6) */
case 6:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand swap (7) */
case 7:
  cpos++;
  break;

/* operand roll (8) */
case 8:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand const (9) */
case 9:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand const_null (10) */
case 10:
  cpos++;
  break;

/* operand const_true (11) */
case 11:
  cpos++;
  break;

/* operand const_false (12) */
case 12:
  cpos++;
  break;

/* operand const_undefined (13) */
case 13:
  cpos++;
  break;

/* operand const_i0 (14) */
case 14:
  cpos++;
  break;

/* operand const_i1 (15) */
case 15:
  cpos++;
  break;

/* operand const_i2 (16) */
case 16:
  cpos++;
  break;

/* operand const_i3 (17) */
case 17:
  cpos++;
  break;

/* operand load_global (18) */
case 18:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand store_global (19) */
case 19:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand load_arg (20) */
case 20:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand store_arg (21) */
case 21:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand load_local (22) */
case 22:
  cpos++;
  cp += 2;
  cpos++;
  break;

/* operand store_local (23) */
case 23:
  cpos++;
  cp += 2;
  cpos++;
  break;

/* operand load_property (24) */
case 24:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand store_property (25) */
case 25:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand load_array (26) */
case 26:
  cpos++;
  break;

/* operand store_array (27) */
case 27:
  cpos++;
  break;

/* operand nth (28) */
case 28:
  cpos++;
  break;

/* operand cmp_eq (29) */
case 29:
  cpos++;
  break;

/* operand cmp_ne (30) */
case 30:
  cpos++;
  break;

/* operand cmp_lt (31) */
case 31:
  cpos++;
  break;

/* operand cmp_gt (32) */
case 32:
  cpos++;
  break;

/* operand cmp_le (33) */
case 33:
  cpos++;
  break;

/* operand cmp_ge (34) */
case 34:
  cpos++;
  break;

/* operand sub (35) */
case 35:
  cpos++;
  break;

/* operand add (36) */
case 36:
  cpos++;
  break;

/* operand mul (37) */
case 37:
  cpos++;
  break;

/* operand div (38) */
case 38:
  cpos++;
  break;

/* operand mod (39) */
case 39:
  cpos++;
  break;

/* operand neg (40) */
case 40:
  cpos++;
  break;

/* operand and (41) */
case 41:
  cpos++;
  break;

/* operand not (42) */
case 42:
  cpos++;
  break;

/* operand or (43) */
case 43:
  cpos++;
  break;

/* operand xor (44) */
case 44:
  cpos++;
  break;

/* operand shift_left (45) */
case 45:
  cpos++;
  break;

/* operand shift_right (46) */
case 46:
  cpos++;
  break;

/* operand shift_rright (47) */
case 47:
  cpos++;
  break;

/* operand iffalse (48) */
case 48:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand iftrue (49) */
case 49:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand call_method (50) */
case 50:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand jmp (51) */
case 51:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand jsr (52) */
case 52:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand return (53) */
case 53:
  cpos++;
  break;

/* operand typeof (54) */
case 54:
  cpos++;
  break;

/* operand new (55) */
case 55:
  cpos++;
  break;

/* operand delete_property (56) */
case 56:
  cpos++;
  cp += 4;
  cpos++;
  break;

/* operand delete_array (57) */
case 57:
  cpos++;
  break;

/* operand locals (58) */
case 58:
  cpos++;
  cp += 2;
  cpos++;
  break;

/* operand min_args (59) */
case 59:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand with_push (60) */
case 60:
  cpos++;
  break;

/* operand with_pop (61) */
case 61:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand try_push (62) */
case 62:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand try_pop (63) */
case 63:
  cpos++;
  cp += 1;
  cpos++;
  break;

/* operand throw (64) */
case 64:
  cpos++;
  break;

/* operand iffalse_b (65) */
case 65:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand iftrue_b (66) */
case 66:
  cpos++;
  i = ARG_INT32 ();
  i = reloc[cp - fixed_code + 4 + i] - &f->code[cpos + 1];
  ARG_INT32 () = i;
  cp += 4;
  cpos++;
  break;

/* operand add_1_i (67) */
case 67:
  cpos++;
  break;

/* operand add_2_i (68) */
case 68:
  cpos++;
  break;

