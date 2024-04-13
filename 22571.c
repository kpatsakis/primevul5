code3_hash(OnigCodePoint* x)
{
  return (st_index_t )(x[0] + x[1] + x[2]);
}