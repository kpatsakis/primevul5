static int get_class_iterator_size(PCRE2_SPTR cc)
{
sljit_u32 min;
sljit_u32 max;
switch(*cc)
  {
  case OP_CRSTAR:
  case OP_CRPLUS:
  return 2;

  case OP_CRMINSTAR:
  case OP_CRMINPLUS:
  case OP_CRQUERY:
  case OP_CRMINQUERY:
  return 1;

  case OP_CRRANGE:
  case OP_CRMINRANGE:
  min = GET2(cc, 1);
  max = GET2(cc, 1 + IMM2_SIZE);
  if (max == 0)
    return (*cc == OP_CRRANGE) ? 2 : 1;
  max -= min;
  if (max > 2)
    max = 2;
  return max;

  default:
  return 0;
  }
}