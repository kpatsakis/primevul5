repodata_insert_keyid(Repodata *data, Id handle, Id keyid, Id val, int overwrite)
{
  Id *pp;
  Id *ap, **app;
  int i;

  app = repodata_get_attrp(data, handle);
  ap = *app;
  i = 0;
  if (ap)
    {
      /* Determine equality based on the name only, allows us to change
         type (when overwrite is set), and makes TYPE_CONSTANT work.  */
      for (pp = ap; *pp; pp += 2)
        if (data->keys[*pp].name == data->keys[keyid].name)
          break;
      if (*pp)
        {
	  if (overwrite || data->keys[*pp].type == REPOKEY_TYPE_DELETED)
	    {
	      pp[0] = keyid;
              pp[1] = val;
	    }
          return;
        }
      i = pp - ap;
    }
  ap = solv_extend(ap, i, 3, sizeof(Id), REPODATA_ATTRS_BLOCK);
  *app = ap;
  pp = ap + i;
  *pp++ = keyid;
  *pp++ = val;
  *pp = 0;
}