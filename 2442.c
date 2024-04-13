lambda_body(codegen_scope *s, node *tree, int blk)
{
  codegen_scope *parent = s;
  s = scope_new(s->mrb, s, tree->car);

  s->mscope = !blk;

  if (blk) {
    struct loopinfo *lp = loop_push(s, LOOP_BLOCK);
    lp->pc0 = new_label(s);
  }
  tree = tree->cdr;
  if (tree->car == NULL) {
    genop_W(s, OP_ENTER, 0);
    s->ainfo = 0;
  }
  else {
    mrb_aspec a;
    int ma, oa, ra, pa, ka, kd, ba, i;
    uint32_t pos;
    node *opt;
    node *margs, *pargs;
    node *tail;

    /* mandatory arguments */
    ma = node_len(tree->car->car);
    margs = tree->car->car;
    tail = tree->car->cdr->cdr->cdr->cdr;

    /* optional arguments */
    oa = node_len(tree->car->cdr->car);
    /* rest argument? */
    ra = tree->car->cdr->cdr->car ? 1 : 0;
    /* mandatory arguments after rest argument */
    pa = node_len(tree->car->cdr->cdr->cdr->car);
    pargs = tree->car->cdr->cdr->cdr->car;
    /* keyword arguments */
    ka = tail? node_len(tail->cdr->car) : 0;
    /* keyword dictionary? */
    kd = tail && tail->cdr->cdr->car? 1 : 0;
    /* block argument? */
    ba = tail && tail->cdr->cdr->cdr->car ? 1 : 0;

    if (ma > 0x1f || oa > 0x1f || pa > 0x1f || ka > 0x1f) {
      codegen_error(s, "too many formal arguments");
    }
    /* (23bits = 5:5:1:5:5:1:1) */
    a = MRB_ARGS_REQ(ma)
      | MRB_ARGS_OPT(oa)
      | (ra? MRB_ARGS_REST() : 0)
      | MRB_ARGS_POST(pa)
      | MRB_ARGS_KEY(ka, kd)
      | (ba? MRB_ARGS_BLOCK() : 0);
    genop_W(s, OP_ENTER, a);
    /* (12bits = 5:1:5:1) */
    s->ainfo = (((ma+oa) & 0x3f) << 7)
      | ((ra & 0x1) << 6)
      | ((pa & 0x1f) << 1)
      | ((ka | kd) ? 1 : 0);
    /* generate jump table for optional arguments initializer */
    pos = new_label(s);
    for (i=0; i<oa; i++) {
      new_label(s);
      genjmp_0(s, OP_JMP);
    }
    if (oa > 0) {
      genjmp_0(s, OP_JMP);
    }
    opt = tree->car->cdr->car;
    i = 0;
    while (opt) {
      int idx;
      mrb_sym id = nsym(opt->car->car);

      dispatch(s, pos+i*3+1);
      codegen(s, opt->car->cdr, VAL);
      pop();
      idx = lv_idx(s, id);
      if (idx > 0) {
        gen_move(s, idx, cursp(), 0);
      }
      else {
        gen_getupvar(s, cursp(), id);
      }
      i++;
      opt = opt->cdr;
    }
    if (oa > 0) {
      dispatch(s, pos+i*3+1);
    }

    /* keyword arguments */
    if (tail) {
      node *kwds = tail->cdr->car;
      int kwrest = 0;

      if (tail->cdr->cdr->car) {
        kwrest = 1;
      }
      mrb_assert(nint(tail->car) == NODE_ARGS_TAIL);
      mrb_assert(node_len(tail) == 4);

      while (kwds) {
        int jmpif_key_p, jmp_def_set = -1;
        node *kwd = kwds->car, *def_arg = kwd->cdr->cdr->car;
        mrb_sym kwd_sym = nsym(kwd->cdr->car);

        mrb_assert(nint(kwd->car) == NODE_KW_ARG);

        if (def_arg) {
          int idx;
          genop_2(s, OP_KEY_P, lv_idx(s, kwd_sym), new_sym(s, kwd_sym));
          jmpif_key_p = genjmp2_0(s, OP_JMPIF, lv_idx(s, kwd_sym), NOVAL);
          codegen(s, def_arg, VAL);
          pop();
          idx = lv_idx(s, kwd_sym);
          if (idx > 0) {
            gen_move(s, idx, cursp(), 0);
          }
          else {
            gen_getupvar(s, cursp(), kwd_sym);
          }
          jmp_def_set = genjmp_0(s, OP_JMP);
          dispatch(s, jmpif_key_p);
        }
        genop_2(s, OP_KARG, lv_idx(s, kwd_sym), new_sym(s, kwd_sym));
        if (jmp_def_set != -1) {
          dispatch(s, jmp_def_set);
        }
        i++;

        kwds = kwds->cdr;
      }
      if (tail->cdr->car && !kwrest) {
        genop_0(s, OP_KEYEND);
      }
    }

    /* argument destructuring */
    if (margs) {
      node *n = margs;

      pos = 1;
      while (n) {
        if (nint(n->car->car) == NODE_MASGN) {
          gen_massignment(s, n->car->cdr->car, pos, NOVAL);
        }
        pos++;
        n = n->cdr;
      }
    }
    if (pargs) {
      node *n = pargs;

      pos = ma+oa+ra+1;
      while (n) {
        if (nint(n->car->car) == NODE_MASGN) {
          gen_massignment(s, n->car->cdr->car, pos, NOVAL);
        }
        pos++;
        n = n->cdr;
      }
    }
  }

  codegen(s, tree->cdr->car, VAL);
  pop();
  if (s->pc > 0) {
    gen_return(s, OP_RETURN, cursp());
  }
  if (blk) {
    loop_pop(s, NOVAL);
  }
  scope_finish(s);
  return parent->irep->rlen - 1;
}