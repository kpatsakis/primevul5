gen_massignment(codegen_scope *s, node *tree, int rhs, int val)
{
  int n = 0, post = 0;
  node *t, *p;

  if (tree->car) {              /* pre */
    t = tree->car;
    n = 0;
    while (t) {
      int sp = cursp();

      genop_3(s, OP_AREF, sp, rhs, n);
      push();
      gen_assignment(s, t->car, NULL, sp, NOVAL);
      pop();
      n++;
      t = t->cdr;
    }
  }
  t = tree->cdr;
  if (t) {
    if (t->cdr) {               /* post count */
      p = t->cdr->car;
      while (p) {
        post++;
        p = p->cdr;
      }
    }
    gen_move(s, cursp(), rhs, val);
    push_n(post+1);
    pop_n(post+1);
    genop_3(s, OP_APOST, cursp(), n, post);
    n = 1;
    if (t->car && t->car != (node*)-1) { /* rest */
      gen_assignment(s, t->car, NULL, cursp(), NOVAL);
    }
    if (t->cdr && t->cdr->car) {
      t = t->cdr->car;
      while (t) {
        gen_assignment(s, t->car, NULL, cursp()+n, NOVAL);
        t = t->cdr;
        n++;
      }
    }
    if (val) {
      gen_move(s, cursp(), rhs, 0);
    }
  }
}