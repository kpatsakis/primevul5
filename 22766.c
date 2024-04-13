onig_print_compiled_byte_code(FILE* f, UChar* bp, UChar* bpend, UChar** nextp,
                              OnigEncoding enc)
{
  int i, n, arg_type;
  RelAddrType addr;
  LengthType len;
  MemNumType mem;
  StateCheckNumType scn;
  OnigCodePoint code;
  UChar *q;

  fprintf(f, "[%s", op2name(*bp));
  arg_type = op2arg_type(*bp);
  if (arg_type != ARG_SPECIAL) {
    bp++;
    switch (arg_type) {
    case ARG_NON:
      break;
    case ARG_RELADDR:
      GET_RELADDR_INC(addr, bp);
      fprintf(f, ":(%d)", addr);
      break;
    case ARG_ABSADDR:
      GET_ABSADDR_INC(addr, bp);
      fprintf(f, ":(%d)", addr);
      break;
    case ARG_LENGTH:
      GET_LENGTH_INC(len, bp);
      fprintf(f, ":%d", len);
      break;
    case ARG_MEMNUM:
      mem = *((MemNumType* )bp);
      bp += SIZE_MEMNUM;
      fprintf(f, ":%d", mem);
      break;
    case ARG_OPTION:
      {
	OnigOptionType option = *((OnigOptionType* )bp);
	bp += SIZE_OPTION;
	fprintf(f, ":%d", option);
      }
      break;

    case ARG_STATE_CHECK:
      scn = *((StateCheckNumType* )bp);
      bp += SIZE_STATE_CHECK_NUM;
      fprintf(f, ":%d", scn);
      break;
    }
  }
  else {
    switch (*bp++) {
    case OP_EXACT1:
    case OP_ANYCHAR_STAR_PEEK_NEXT:
    case OP_ANYCHAR_ML_STAR_PEEK_NEXT:
      p_string(f, 1, bp++); break;
    case OP_EXACT2:
      p_string(f, 2, bp); bp += 2; break;
    case OP_EXACT3:
      p_string(f, 3, bp); bp += 3; break;
    case OP_EXACT4:
      p_string(f, 4, bp); bp += 4; break;
    case OP_EXACT5:
      p_string(f, 5, bp); bp += 5; break;
    case OP_EXACTN:
      GET_LENGTH_INC(len, bp);
      p_len_string(f, len, 1, bp);
      bp += len;
      break;

    case OP_EXACTMB2N1:
      p_string(f, 2, bp); bp += 2; break;
    case OP_EXACTMB2N2:
      p_string(f, 4, bp); bp += 4; break;
    case OP_EXACTMB2N3:
      p_string(f, 6, bp); bp += 6; break;
    case OP_EXACTMB2N:
      GET_LENGTH_INC(len, bp);
      p_len_string(f, len, 2, bp);
      bp += len * 2;
      break;
    case OP_EXACTMB3N:
      GET_LENGTH_INC(len, bp);
      p_len_string(f, len, 3, bp);
      bp += len * 3;
      break;
    case OP_EXACTMBN:
      {
	int mb_len;

	GET_LENGTH_INC(mb_len, bp);
	GET_LENGTH_INC(len, bp);
	fprintf(f, ":%d:%d:", mb_len, len);
	n = len * mb_len;
	while (n-- > 0) { fputc(*bp++, f); }
      }
      break;

    case OP_EXACT1_IC:
      len = enclen(enc, bp, bpend);
      p_string(f, len, bp);
      bp += len;
      break;
    case OP_EXACTN_IC:
      GET_LENGTH_INC(len, bp);
      p_len_string(f, len, 1, bp);
      bp += len;
      break;

    case OP_CCLASS:
      n = bitset_on_num((BitSetRef )bp);
      bp += SIZE_BITSET;
      fprintf(f, ":%d", n);
      break;

    case OP_CCLASS_NOT:
      n = bitset_on_num((BitSetRef )bp);
      bp += SIZE_BITSET;
      fprintf(f, ":%d", n);
      break;

    case OP_CCLASS_MB:
    case OP_CCLASS_MB_NOT:
      GET_LENGTH_INC(len, bp);
      q = bp;
#ifndef PLATFORM_UNALIGNED_WORD_ACCESS
      ALIGNMENT_RIGHT(q);
#endif
      GET_CODE_POINT(code, q);
      bp += len;
      fprintf(f, ":%d:%d", (int )code, len);
      break;

    case OP_CCLASS_MIX:
    case OP_CCLASS_MIX_NOT:
      n = bitset_on_num((BitSetRef )bp);
      bp += SIZE_BITSET;
      GET_LENGTH_INC(len, bp);
      q = bp;
#ifndef PLATFORM_UNALIGNED_WORD_ACCESS
      ALIGNMENT_RIGHT(q);
#endif
      GET_CODE_POINT(code, q);
      bp += len;
      fprintf(f, ":%d:%d:%d", n, (int )code, len);
      break;

    case OP_CCLASS_NODE:
      {
        CClassNode *cc;

        GET_POINTER_INC(cc, bp);
        n = bitset_on_num(cc->bs);
        fprintf(f, ":%"PRIuPTR":%d", (uintptr_t)cc, n);
      }
      break;

    case OP_BACKREFN_IC:
      mem = *((MemNumType* )bp);
      bp += SIZE_MEMNUM;
      fprintf(f, ":%d", mem);
      break;

    case OP_BACKREF_MULTI_IC:
    case OP_BACKREF_MULTI:
      fputs(" ", f);
      GET_LENGTH_INC(len, bp);
      for (i = 0; i < len; i++) {
	GET_MEMNUM_INC(mem, bp);
	if (i > 0) fputs(", ", f);
	fprintf(f, "%d", mem);
      }
      break;

    case OP_BACKREF_WITH_LEVEL:
      {
	OnigOptionType option;
	LengthType level;

	GET_OPTION_INC(option, bp);
	fprintf(f, ":%d", option);
	GET_LENGTH_INC(level, bp);
	fprintf(f, ":%d", level);

	fputs(" ", f);
	GET_LENGTH_INC(len, bp);
	for (i = 0; i < len; i++) {
	  GET_MEMNUM_INC(mem, bp);
	  if (i > 0) fputs(", ", f);
	  fprintf(f, "%d", mem);
	}
      }
      break;

    case OP_REPEAT:
    case OP_REPEAT_NG:
      {
	mem = *((MemNumType* )bp);
	bp += SIZE_MEMNUM;
	addr = *((RelAddrType* )bp);
	bp += SIZE_RELADDR;
	fprintf(f, ":%d:%d", mem, addr);
      }
      break;

    case OP_PUSH_OR_JUMP_EXACT1:
    case OP_PUSH_IF_PEEK_NEXT:
      addr = *((RelAddrType* )bp);
      bp += SIZE_RELADDR;
      fprintf(f, ":(%d)", addr);
      p_string(f, 1, bp);
      bp += 1;
      break;

    case OP_LOOK_BEHIND:
      GET_LENGTH_INC(len, bp);
      fprintf(f, ":%d", len);
      break;

    case OP_PUSH_LOOK_BEHIND_NOT:
      GET_RELADDR_INC(addr, bp);
      GET_LENGTH_INC(len, bp);
      fprintf(f, ":%d:(%d)", len, addr);
      break;

    case OP_STATE_CHECK_PUSH:
    case OP_STATE_CHECK_PUSH_OR_JUMP:
      scn = *((StateCheckNumType* )bp);
      bp += SIZE_STATE_CHECK_NUM;
      addr = *((RelAddrType* )bp);
      bp += SIZE_RELADDR;
      fprintf(f, ":%d:(%d)", scn, addr);
      break;

    default:
      fprintf(stderr, "onig_print_compiled_byte_code: undefined code %d\n",
	      *--bp);
    }
  }
  fputs("]", f);
  if (nextp) *nextp = bp;
}