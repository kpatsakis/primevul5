repodata_unset_uninternalized(Repodata *data, Id solvid, Id keyname)
{
  Id *pp, *ap, **app;
  app = repodata_get_attrp(data, solvid);
  ap = *app;
  if (!ap)
    return;
  if (!keyname)
    {
      *app = 0;		/* delete all attributes */
      return;
    }
  for (; *ap; ap += 2)
    if (data->keys[*ap].name == keyname)
      break;
  if (!*ap)
    return;
  pp = ap;
  ap += 2;
  for (; *ap; ap += 2)
    {
      if (data->keys[*ap].name == keyname)
	continue;
      *pp++ = ap[0];
      *pp++ = ap[1];
    }
  *pp = 0;
}