gen_call(codegen_scope *s, node *tree, int val, int safe)
{
  mrb_sym sym = nsym(tree->cdr->car);
  int skip = 0, n = 0, nk = 0, noop = no_optimize(s), noself = 0, blk = 0, sp_save = cursp();

  if (!tree->car) {
    noself = noop = 1;
    push();
  }
  else {
    codegen(s, tree->car, VAL); /* receiver */
  }
  if (safe) {
    int recv = cursp()-1;
    gen_move(s, cursp(), recv, 1);
    skip = genjmp2_0(s, OP_JMPNIL, cursp(), val);
  }
  tree = tree->cdr->cdr->car;
  if (tree) {
    if (tree->car) {            /* positional arguments */
      n = gen_values(s, tree->car, VAL, 14);
      if (n < 0) {              /* variable length */
        noop = 1;               /* not operator */
        n = 15;
        push();
      }
    }
    if (tree->cdr->car) {       /* keyword arguments */
      noop = 1;
      nk = gen_hash(s, tree->cdr->car->cdr, VAL, 14);
      if (nk < 0) nk = 15;
    }
  }
  if (tree && tree->cdr && tree->cdr->cdr) {
    codegen(s, tree->cdr->cdr, VAL);
    pop();
    noop = 1;
    blk = 1;
  }
  push();pop();
  s->sp = sp_save;
  if (!noop && sym == MRB_OPSYM_2(s->mrb, add) && n == 1)  {
    gen_addsub(s, OP_ADD, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, sub) && n == 1)  {
    gen_addsub(s, OP_SUB, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, mul) && n == 1)  {
    gen_muldiv(s, OP_MUL, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, div) && n == 1)  {
    gen_muldiv(s, OP_DIV, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, lt) && n == 1)  {
    genop_1(s, OP_LT, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, le) && n == 1)  {
    genop_1(s, OP_LE, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, gt) && n == 1)  {
    genop_1(s, OP_GT, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, ge) && n == 1)  {
    genop_1(s, OP_GE, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, eq) && n == 1)  {
    genop_1(s, OP_EQ, cursp());
  }
  else if (!noop && sym == MRB_OPSYM_2(s->mrb, aset) && n == 2)  {
    genop_1(s, OP_SETIDX, cursp());
  }
  else if (!noop && n == 0 && gen_uniop(s, sym, cursp())) {
    /* constant folding succeeded */
  }
  else if (!noop && n == 1 && gen_binop(s, sym, cursp())) {
    /* constant folding succeeded */
  }
  else if (noself){
    genop_3(s, blk ? OP_SSENDB : OP_SSEND, cursp(), new_sym(s, sym), n|(nk<<4));
  }
  else {
    genop_3(s, blk ? OP_SENDB : OP_SEND, cursp(), new_sym(s, sym), n|(nk<<4));
  }
  if (safe) {
    dispatch(s, skip);
  }
  if (val) {
    push();
  }
}