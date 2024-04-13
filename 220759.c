repodata_internalize(Repodata *data)
{
  Repokey *key, solvkey;
  Id entry, nentry;
  Id schemaid, keyid, *schema, *sp, oldschemaid, *keyp, *seen;
  Offset *oldincoreoffs = 0;
  int schemaidx;
  unsigned char *dp, *ndp;
  int neednewschema;
  struct extdata newincore;
  struct extdata newvincore;
  Id solvkeyid;
  Id *keylink;
  int haveoldkl;

  if (!data->attrs && !data->xattrs)
    return;

#if 0
  printf("repodata_internalize %d\n", data->repodataid);
  printf("  attr data: %d K\n", data->attrdatalen / 1024);
  printf("  attrid data: %d K\n", data->attriddatalen / (1024 / 4));
#endif
  newvincore.buf = data->vincore;
  newvincore.len = data->vincorelen;

  /* find the solvables key, create if needed */
  memset(&solvkey, 0, sizeof(solvkey));
  solvkey.name = REPOSITORY_SOLVABLES;
  solvkey.type = REPOKEY_TYPE_FLEXARRAY;
  solvkey.size = 0;
  solvkey.storage = KEY_STORAGE_INCORE;
  solvkeyid = repodata_key2id(data, &solvkey, data->end != data->start ? 1 : 0);

  schema = solv_malloc2(data->nkeys, sizeof(Id));
  seen = solv_malloc2(data->nkeys, sizeof(Id));

  /* Merge the data already existing (in data->schemata, ->incoredata and
     friends) with the new attributes in data->attrs[].  */
  nentry = data->end - data->start;
  memset(&newincore, 0, sizeof(newincore));
  data_addid(&newincore, 0);	/* start data at offset 1 */

  data->mainschema = 0;
  data->mainschemaoffsets = solv_free(data->mainschemaoffsets);

  keylink = calculate_keylink(data);
  /* join entry data */
  /* we start with the meta data, entry -1 */
  for (entry = -1; entry < nentry; entry++)
    {
      oldschemaid = 0;
      dp = data->incoredata;
      if (dp)
	{
	  dp += entry >= 0 ? data->incoreoffset[entry] : 1;
          dp = data_read_id(dp, &oldschemaid);
	}
      memset(seen, 0, data->nkeys * sizeof(Id));
#if 0
fprintf(stderr, "oldschemaid %d\n", oldschemaid);
fprintf(stderr, "schemata %d\n", data->schemata[oldschemaid]);
fprintf(stderr, "schemadata %p\n", data->schemadata);
#endif

      /* seen: -1: old data,  0: skipped,  >0: id + 1 */
      neednewschema = 0;
      sp = schema;
      haveoldkl = 0;
      for (keyp = data->schemadata + data->schemata[oldschemaid]; *keyp; keyp++)
	{
	  if (seen[*keyp])
	    {
	      /* oops, should not happen */
	      neednewschema = 1;
	      continue;
	    }
	  seen[*keyp] = -1;	/* use old marker */
	  *sp++ = *keyp;
	  if (keylink[*keyp])
	    haveoldkl = 1;	/* potential keylink conflict */
	}

      /* strip solvables key */
      if (entry < 0 && solvkeyid && seen[solvkeyid])
	{
	  *sp = 0;
	  for (sp = keyp = schema; *sp; sp++)
	    if (*sp != solvkeyid)
	      *keyp++ = *sp;
	  sp = keyp;
	  seen[solvkeyid] = 0;
	  neednewschema = 1;
	}

      /* add new entries */
      if (entry >= 0)
	keyp = data->attrs ? data->attrs[entry] : 0;
      else
        keyp = data->xattrs ? data->xattrs[1] : 0;
      if (keyp)
        for (; *keyp; keyp += 2)
	  {
	    if (!seen[*keyp])
	      {
	        neednewschema = 1;
	        *sp++ = *keyp;
		if (haveoldkl && keylink[*keyp])		/* this should be pretty rare */
		  {
		    Id kl;
		    for (kl = keylink[*keyp]; kl != *keyp; kl = keylink[kl])
		      if (seen[kl] == -1)
		        {
			  /* replacing old key kl, remove from schema and seen */
			  Id *osp;
			  for (osp = schema; osp < sp; osp++)
			    if (*osp == kl)
			      {
			        memmove(osp, osp + 1, (sp - osp) * sizeof(Id));
			        sp--;
			        seen[kl] = 0;
				break;
			      }
		        }
		  }
	      }
	    seen[*keyp] = keyp[1] + 1;
	  }

      /* add solvables key if needed */
      if (entry < 0 && data->end != data->start)
	{
	  *sp++ = solvkeyid;	/* always last in schema */
	  neednewschema = 1;
	}

      /* commit schema */
      *sp = 0;
      if (neednewschema)
        /* Ideally we'd like to sort the new schema here, to ensure
	   schema equality independend of the ordering. */
	schemaid = repodata_schema2id(data, schema, 1);
      else
	schemaid = oldschemaid;

      if (entry < 0)
	{
	  data->mainschemaoffsets = solv_calloc(sp - schema, sizeof(Id));
	  data->mainschema = schemaid;
	}

      /* find offsets in old incore data */
      if (oldschemaid)
	{
	  Id *lastneeded = 0;
	  for (sp = data->schemadata + data->schemata[oldschemaid]; *sp; sp++)
	    if (seen[*sp] == -1)
	      lastneeded = sp + 1;
	  if (lastneeded)
	    {
	      if (!oldincoreoffs)
	        oldincoreoffs = solv_malloc2(data->nkeys, 2 * sizeof(Offset));
	      for (sp = data->schemadata + data->schemata[oldschemaid]; sp != lastneeded; sp++)
		{
		  /* Skip the data associated with this old key.  */
		  key = data->keys + *sp;
		  ndp = dp;
		  if (key->storage == KEY_STORAGE_VERTICAL_OFFSET)
		    {
		      ndp = data_skip(ndp, REPOKEY_TYPE_ID);
		      ndp = data_skip(ndp, REPOKEY_TYPE_ID);
		    }
		  else if (key->storage == KEY_STORAGE_INCORE)
		    ndp = data_skip_key(data, ndp, key);
		  oldincoreoffs[*sp * 2] = dp - data->incoredata;
		  oldincoreoffs[*sp * 2 + 1] = ndp - dp;
		  dp = ndp;
		}
	    }
	}

      /* just copy over the complete old entry (including the schemaid) if there was no new data */
      if (entry >= 0 && !neednewschema && oldschemaid && (!data->attrs || !data->attrs[entry]) && dp)
	{
	  ndp = data->incoredata + data->incoreoffset[entry];
	  data->incoreoffset[entry] = newincore.len;
	  data_addblob(&newincore, ndp, dp - ndp);
	  goto entrydone;
	}

      /* Now create data blob.  We walk through the (possibly new) schema
	 and either copy over old data, or insert the new.  */
      if (entry >= 0)
        data->incoreoffset[entry] = newincore.len;
      data_addid(&newincore, schemaid);

      /* we don't use a pointer to the schemadata here as repodata_serialize_key
       * may call repodata_schema2id() which might realloc our schemadata */
      for (schemaidx = data->schemata[schemaid]; (keyid = data->schemadata[schemaidx]) != 0; schemaidx++)
	{
	  if (entry < 0)
	    {
	      data->mainschemaoffsets[schemaidx - data->schemata[schemaid]] = newincore.len;
	      if (keyid == solvkeyid)
		{
		  /* add flexarray entry count */
		  data_addid(&newincore, data->end - data->start);
		  break;	/* always the last entry */
		}
	    }
	  if (seen[keyid] == -1)
	    {
	      if (oldincoreoffs[keyid * 2 + 1])
		data_addblob(&newincore, data->incoredata + oldincoreoffs[keyid * 2], oldincoreoffs[keyid * 2 + 1]);
	    }
	  else if (seen[keyid])
	    repodata_serialize_key(data, &newincore, &newvincore, schema, data->keys + keyid, seen[keyid] - 1);
	}

entrydone:
      /* free memory */
      if (entry >= 0 && data->attrs)
	{
	  if (data->attrs[entry])
	    data->attrs[entry] = solv_free(data->attrs[entry]);
	  if (entry && entry % 4096 == 0 && data->nxattrs <= 2 && entry + 64 < nentry)
	    {
	      compact_attrdata(data, entry + 1, nentry);	/* try to free some memory */
#if 0
	      printf("  attr data: %d K\n", data->attrdatalen / 1024);
	      printf("  attrid data: %d K\n", data->attriddatalen / (1024 / 4));
	      printf("  incore data: %d K\n", newincore.len / 1024);
	      printf("  sum: %d K\n", (newincore.len + data->attrdatalen + data->attriddatalen * 4) / 1024);
	      /* malloc_stats(); */
#endif
	    }
	}
    }
  /* free all xattrs */
  for (entry = 0; entry < data->nxattrs; entry++)
    if (data->xattrs[entry])
      solv_free(data->xattrs[entry]);
  data->xattrs = solv_free(data->xattrs);
  data->nxattrs = 0;

  data->lasthandle = 0;
  data->lastkey = 0;
  data->lastdatalen = 0;
  solv_free(schema);
  solv_free(seen);
  solv_free(keylink);
  solv_free(oldincoreoffs);
  repodata_free_schemahash(data);

  solv_free(data->incoredata);
  data->incoredata = newincore.buf;
  data->incoredatalen = newincore.len;
  data->incoredatafree = 0;

  data->vincore = newvincore.buf;
  data->vincorelen = newvincore.len;

  data->attrs = solv_free(data->attrs);
  data->attrdata = solv_free(data->attrdata);
  data->attriddata = solv_free(data->attriddata);
  data->attrnum64data = solv_free(data->attrnum64data);
  data->attrdatalen = 0;
  data->attriddatalen = 0;
  data->attrnum64datalen = 0;
#if 0
  printf("repodata_internalize %d done\n", data->repodataid);
  printf("  incore data: %d K\n", data->incoredatalen / 1024);
#endif
}