noname_disable_map(Node** plink, GroupNumRemap* map, int* counter)
{
  int r = 0;
  Node* node = *plink;

  switch (NTYPE(node)) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = noname_disable_map(&(NCAR(node)), map, counter);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;

  case NT_QTFR:
    {
      Node** ptarget = &(NQTFR(node)->target);
      Node*  old = *ptarget;
      r = noname_disable_map(ptarget, map, counter);
      if (*ptarget != old && NTYPE(*ptarget) == NT_QTFR) {
	onig_reduce_nested_quantifier(node, *ptarget);
      }
    }
    break;

  case NT_ENCLOSE:
    {
      EncloseNode* en = NENCLOSE(node);
      if (en->type == ENCLOSE_MEMORY) {
	if (IS_ENCLOSE_NAMED_GROUP(en)) {
	  (*counter)++;
	  map[en->regnum].new_val = *counter;
	  en->regnum = *counter;
	  r = noname_disable_map(&(en->target), map, counter);
	}
	else {
	  *plink = en->target;
	  en->target = NULL_NODE;
	  onig_node_free(node);
	  r = noname_disable_map(plink, map, counter);
	}
      }
      else
	r = noname_disable_map(&(en->target), map, counter);
    }
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);
      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = noname_disable_map(&(an->target), map, counter);
	break;
      }
    }
    break;

  default:
    break;
  }

  return r;
}