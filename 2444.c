gen_move(codegen_scope *s, uint16_t dst, uint16_t src, int nopeep)
{
  if (nopeep || no_peephole(s)) goto normal;
  else if (dst == src) return;
  else {
    struct mrb_insn_data data = mrb_last_insn(s);

    switch (data.insn) {
    case OP_MOVE:
      if (dst == src) return;   /* remove useless MOVE */
      if (data.a == src) {
        if (data.b == dst)      /* skip swapping MOVE */
          return;
        if (data.a < s->nlocals) goto normal;
        rewind_pc(s);
        s->lastpc = addr_pc(s, mrb_prev_pc(s, data.addr));
        gen_move(s, dst, data.b, FALSE);
        return;
      }
      if (dst == data.a) {      /* skip overwritten move */
        rewind_pc(s);
        s->lastpc = addr_pc(s, mrb_prev_pc(s, data.addr));
        gen_move(s, dst, src, FALSE);
        return;
      }
      goto normal;
    case OP_LOADNIL: case OP_LOADSELF: case OP_LOADT: case OP_LOADF:
    case OP_LOADI__1:
    case OP_LOADI_0: case OP_LOADI_1: case OP_LOADI_2: case OP_LOADI_3:
    case OP_LOADI_4: case OP_LOADI_5: case OP_LOADI_6: case OP_LOADI_7:
      if (data.a != src || data.a < s->nlocals) goto normal;
      rewind_pc(s);
      genop_1(s, data.insn, dst);
      return;
    case OP_HASH:
      if (data.b != 0) goto normal;
      /* fall through */
    case OP_LOADI: case OP_LOADINEG:
    case OP_LOADL: case OP_LOADSYM:
    case OP_GETGV: case OP_GETSV: case OP_GETIV: case OP_GETCV:
    case OP_GETCONST: case OP_STRING:
    case OP_LAMBDA: case OP_BLOCK: case OP_METHOD: case OP_BLKPUSH:
      if (data.a != src || data.a < s->nlocals) goto normal;
      rewind_pc(s);
      genop_2(s, data.insn, dst, data.b);
      return;
    case OP_LOADI16:
      if (data.a != src || data.a < s->nlocals) goto normal;
      rewind_pc(s);
      genop_2S(s, data.insn, dst, data.b);
      return;
    case OP_LOADI32:
      if (data.a != src || data.a < s->nlocals) goto normal;
      else {
        uint32_t i = (uint32_t)data.b<<16|data.c;
        rewind_pc(s);
        genop_2SS(s, data.insn, dst, i);
      }
      return;
    case OP_ARRAY:
      if (data.a != src || data.a < s->nlocals || data.a < dst) goto normal;
      rewind_pc(s);
      if (data.b == 0 || dst == data.a)
        genop_2(s, OP_ARRAY, dst, 0);
      else
        genop_3(s, OP_ARRAY2, dst, data.a, data.b);
      return;
    case OP_ARRAY2:
      if (data.a != src || data.a < s->nlocals || data.a < dst) goto normal;
      rewind_pc(s);
      genop_3(s, OP_ARRAY2, dst, data.b, data.c);
      return;
    case OP_AREF:
    case OP_GETUPVAR:
      if (data.a != src || data.a < s->nlocals) goto normal;
      rewind_pc(s);
      genop_3(s, data.insn, dst, data.b, data.c);
      return;
    case OP_ADDI: case OP_SUBI:
      if (addr_pc(s, data.addr) == s->lastlabel || data.a != src || data.a < s->nlocals) goto normal;
      else {
        struct mrb_insn_data data0 = mrb_decode_insn(mrb_prev_pc(s, data.addr));
        if (data0.insn != OP_MOVE || data0.a != data.a || data0.b != dst) goto normal;
        s->pc = addr_pc(s, data0.addr);
        if (addr_pc(s, data0.addr) != s->lastlabel) {
          /* constant folding */
          data0 = mrb_decode_insn(mrb_prev_pc(s, data0.addr));
          mrb_int n;
          if (data0.a == dst && get_int_operand(s, &data0, &n)) {
            if ((data.insn == OP_ADDI && !mrb_int_add_overflow(n, data.b, &n)) ||
                (data.insn == OP_SUBI && !mrb_int_sub_overflow(n, data.b, &n))) {
              s->pc = addr_pc(s, data0.addr);
              gen_int(s, dst, n);
              return;
            }
          }
        }
      }
      genop_2(s, data.insn, dst, data.b);
      return;
    default:
      break;
    }
  }
 normal:
  genop_2(s, OP_MOVE, dst, src);
  return;
}