dataiterator_entersub(Dataiterator *di)
{
  if (di->state == di_nextarrayelement)
    di->state = di_entersub;
}