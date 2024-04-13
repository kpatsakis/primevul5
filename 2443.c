gen_muldiv(codegen_scope *s, uint8_t op, uint16_t dst)
{
  if (no_peephole(s)) {
  normal:
    genop_1(s, op, dst);
    return;
  }
  else {
    struct mrb_insn_data data = mrb_last_insn(s);
    mrb_int n, n0;
    if (addr_pc(s, data.addr) == s->lastlabel || !get_int_operand(s, &data, &n)) {
      /* not integer immediate */
      goto normal;
    }
    struct mrb_insn_data data0 = mrb_decode_insn(mrb_prev_pc(s, data.addr));
    if (!get_int_operand(s, &data0, &n0)) {
      goto normal;
    }
    if (op == OP_MUL) {
      if (mrb_int_mul_overflow(n0, n, &n)) goto normal;
    }
    else { /* OP_DIV */
      if (n == 0) goto normal;
      if (n0 == MRB_INT_MIN && n == -1) goto normal;
      n = mrb_div_int(s->mrb, n0, n);
    }
    s->pc = addr_pc(s, data0.addr);
    gen_int(s, dst, n);
  }
}