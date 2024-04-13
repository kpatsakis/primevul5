for_body(codegen_scope *s, node *tree)
{
  codegen_scope *prev = s;
  int idx;
  struct loopinfo *lp;
  node *n2;

  /* generate receiver */
  codegen(s, tree->cdr->car, VAL);
  /* generate loop-block */
  s = scope_new(s->mrb, s, NULL);

  push();                       /* push for a block parameter */

  /* generate loop variable */
  n2 = tree->car;
  genop_W(s, OP_ENTER, 0x40000);
  if (n2->car && !n2->car->cdr && !n2->cdr) {
    gen_assignment(s, n2->car->car, NULL, 1, NOVAL);
  }
  else {
    gen_massignment(s, n2, 1, VAL);
  }
  /* construct loop */
  lp = loop_push(s, LOOP_FOR);
  lp->pc1 = new_label(s);

  /* loop body */
  codegen(s, tree->cdr->cdr->car, VAL);
  pop();
  gen_return(s, OP_RETURN, cursp());
  loop_pop(s, NOVAL);
  scope_finish(s);
  s = prev;
  genop_2(s, OP_BLOCK, cursp(), s->irep->rlen-1);
  push();pop(); /* space for a block */
  pop();
  idx = new_sym(s, MRB_SYM_2(s->mrb, each));
  genop_3(s, OP_SENDB, cursp(), idx, 0);
}