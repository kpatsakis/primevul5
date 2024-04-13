renumber_by_map(Node* node, GroupNumRemap* map)
{
  int r = 0;

  switch (NTYPE(node)) {
  case NT_LIST:
  case NT_ALT:
    do {
      r = renumber_by_map(NCAR(node), map);
    } while (r == 0 && IS_NOT_NULL(node = NCDR(node)));
    break;
  case NT_QTFR:
    r = renumber_by_map(NQTFR(node)->target, map);
    break;
  case NT_ENCLOSE:
    r = renumber_by_map(NENCLOSE(node)->target, map);
    break;

  case NT_BREF:
    r = renumber_node_backref(node, map);
    break;

  case NT_ANCHOR:
    {
      AnchorNode* an = NANCHOR(node);
      switch (an->type) {
      case ANCHOR_PREC_READ:
      case ANCHOR_PREC_READ_NOT:
      case ANCHOR_LOOK_BEHIND:
      case ANCHOR_LOOK_BEHIND_NOT:
	r = renumber_by_map(an->target, map);
	break;
      }
    }
    break;

  default:
    break;
  }

  return r;
}