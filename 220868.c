dataiterator_skip_attribute(Dataiterator *di)
{
  if (di->state == di_nextsolvableattr)
    di->state = di_nextsolvablekey;
  else
    di->state = di_nextkey;
}