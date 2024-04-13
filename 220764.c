dataiterator_free(Dataiterator *di)
{
  if (di->matcher.match)
    datamatcher_free(&di->matcher);
  if (di->dupstr)
    solv_free(di->dupstr);
  if (di->oldkeyskip)
    solv_free(di->oldkeyskip);
}