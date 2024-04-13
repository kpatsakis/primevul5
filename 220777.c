dataiterator_step(Dataiterator *di)
{
  Id schema;

  if (di->state == di_nextattr && di->key->storage == KEY_STORAGE_VERTICAL_OFFSET && di->vert_ddp && di->vert_storestate != di->data->storestate)
    {
      unsigned int ddpoff = di->ddp - di->vert_ddp;
      di->vert_off += ddpoff;
      di->vert_len -= ddpoff;
      di->ddp = di->vert_ddp = get_vertical_data(di->data, di->key, di->vert_off, di->vert_len);
      di->vert_storestate = di->data->storestate;
      if (!di->ddp)
	di->state = di_nextkey;
    }
  for (;;)
    {
      switch (di->state)
	{
	case di_enterrepo: di_enterrepo:
	  if (!di->repo || (di->repo->disabled && !(di->flags & SEARCH_DISABLED_REPOS)))
	    goto di_nextrepo;
	  if (!(di->flags & SEARCH_THISSOLVID))
	    {
	      di->solvid = di->repo->start - 1;	/* reset solvid iterator */
	      goto di_nextsolvable;
	    }
	  /* FALLTHROUGH */

	case di_entersolvable: di_entersolvable:
	  if (!di->repodataid)
	    goto di_enterrepodata;	/* POS case, repodata is set */
	  if (di->solvid > 0 && !(di->flags & SEARCH_NO_STORAGE_SOLVABLE) && (!di->keyname || (di->keyname >= SOLVABLE_NAME && di->keyname <= RPM_RPMDBID)) && di->nparents - di->rootlevel == di->nkeynames)
	    {
	      extern Repokey repo_solvablekeys[RPM_RPMDBID - SOLVABLE_NAME + 1];
	      di->key = repo_solvablekeys + (di->keyname ? di->keyname - SOLVABLE_NAME : 0);
	      di->data = 0;
	      goto di_entersolvablekey;
	    }

	  if (di->keyname)
	    {
	      di->data = di->keyname == SOLVABLE_FILELIST ? repo_lookup_filelist_repodata(di->repo, di->solvid, &di->matcher) : repo_lookup_repodata_opt(di->repo, di->solvid, di->keyname);
	      if (!di->data)
		goto di_nextsolvable;
	      di->repodataid = di->data - di->repo->repodata;
	      di->keyskip = 0;
	      goto di_enterrepodata;
	    }
	di_leavesolvablekey:
	  di->repodataid = 1;	/* reset repodata iterator */
	  di->keyskip = repo_create_keyskip(di->repo, di->solvid, &di->oldkeyskip);
	  /* FALLTHROUGH */

	case di_enterrepodata: di_enterrepodata:
	  if (di->repodataid)
	    {
	      if (di->repodataid >= di->repo->nrepodata)
		goto di_nextsolvable;
	      di->data = di->repo->repodata + di->repodataid;
	    }
	  if (!maybe_load_repodata(di->data, di->keyname))
	    goto di_nextrepodata;
	  di->dp = solvid2data(di->data, di->solvid, &schema);
	  if (!di->dp)
	    goto di_nextrepodata;
	  if (di->solvid == SOLVID_POS)
	    di->solvid = di->pool->pos.solvid;
	  /* reset key iterator */
	  di->keyp = di->data->schemadata + di->data->schemata[schema];
	  /* FALLTHROUGH */

	case di_enterschema: di_enterschema:
	  if (di->keyname)
	    di->dp = dataiterator_find_keyname(di, di->keyname);
	  if (!di->dp || !*di->keyp)
	    {
	      if (di->kv.parent)
		goto di_leavesub;
	      goto di_nextrepodata;
	    }
	  /* FALLTHROUGH */

	case di_enterkey: di_enterkey:
	  di->kv.entry = -1;
	  di->key = di->data->keys + *di->keyp;
	  if (!di->dp)
	    goto di_nextkey;
	  /* this is get_data() modified to store vert_ data */
	  if (di->key->storage == KEY_STORAGE_VERTICAL_OFFSET)
	    {
	      Id off, len;
	      di->dp = data_read_id(di->dp, &off);
	      di->dp = data_read_id(di->dp, &len);
	      di->vert_ddp = di->ddp = get_vertical_data(di->data, di->key, off, len);
	      di->vert_off = off;
	      di->vert_len = len;
	      di->vert_storestate = di->data->storestate;
	    }
	  else if (di->key->storage == KEY_STORAGE_INCORE)
	    {
	      di->ddp = di->dp;		/* start of data */
	      if (di->keyp[1] && (!di->keyname || (di->flags & SEARCH_SUB) != 0))
		di->dp = data_skip_key(di->data, di->dp, di->key);	/* advance to next key */
	    }
	  else
	    di->ddp = 0;
	  if (!di->ddp)
	    goto di_nextkey;
	  if (di->keyskip && (di->key->name >= di->keyskip[0] || di->keyskip[3 + di->key->name] != di->keyskip[1] + di->data->repodataid))
	    goto di_nextkey;
          if (di->key->type == REPOKEY_TYPE_DELETED && !(di->flags & SEARCH_KEEP_TYPE_DELETED))
	    goto di_nextkey;
	  if (di->key->type == REPOKEY_TYPE_FIXARRAY || di->key->type == REPOKEY_TYPE_FLEXARRAY)
	    goto di_enterarray;
	  if (di->nkeynames && di->nparents - di->rootlevel < di->nkeynames)
	    goto di_nextkey;
	  /* FALLTHROUGH */

	case di_nextattr:
          di->kv.entry++;
	  di->ddp = data_fetch(di->ddp, &di->kv, di->key);
	  di->state = di->kv.eof ? di_nextkey : di_nextattr;
	  break;

	case di_nextkey: di_nextkey:
	  if (!di->keyname && *++di->keyp)
	    goto di_enterkey;
	  if (di->kv.parent)
	    goto di_leavesub;
	  /* FALLTHROUGH */

	case di_nextrepodata: di_nextrepodata:
	  if (!di->keyname && di->repodataid && ++di->repodataid < di->repo->nrepodata)
	      goto di_enterrepodata;
	  /* FALLTHROUGH */

	case di_nextsolvable: di_nextsolvable:
	  if (!(di->flags & SEARCH_THISSOLVID))
	    {
	      if (di->solvid < 0)
		di->solvid = di->repo->start;
	      else
	        di->solvid++;
	      for (; di->solvid < di->repo->end; di->solvid++)
		{
		  if (di->pool->solvables[di->solvid].repo == di->repo)
		    goto di_entersolvable;
		}
	    }
	  /* FALLTHROUGH */

	case di_nextrepo: di_nextrepo:
	  if (di->repoid > 0)
	    {
	      di->repoid++;
	      di->repodataid = 1;
	      if (di->repoid < di->pool->nrepos)
		{
		  di->repo = di->pool->repos[di->repoid];
	          goto di_enterrepo;
		}
	    }
	/* FALLTHROUGH */

	case di_bye: di_bye:
	  di->state = di_bye;
	  return 0;

	case di_enterarray: di_enterarray:
	  if (di->key->name == REPOSITORY_SOLVABLES)
	    goto di_nextkey;
	  di->ddp = data_read_id(di->ddp, (Id *)&di->kv.num);
	  di->kv.eof = 0;
	  di->kv.entry = -1;
	  /* FALLTHROUGH */

	case di_nextarrayelement: di_nextarrayelement:
	  di->kv.entry++;
	  if (di->kv.entry)
	    di->ddp = data_skip_schema(di->data, di->ddp, di->kv.id);
	  if (di->kv.entry == di->kv.num)
	    {
	      if (di->nkeynames && di->nparents - di->rootlevel < di->nkeynames)
		goto di_nextkey;
	      if (!(di->flags & SEARCH_ARRAYSENTINEL))
		goto di_nextkey;
	      di->kv.str = (char *)di->ddp;
	      di->kv.eof = 2;
	      di->state = di_nextkey;
	      break;
	    }
	  if (di->kv.entry == di->kv.num - 1)
	    di->kv.eof = 1;
	  if (di->key->type == REPOKEY_TYPE_FLEXARRAY || !di->kv.entry)
	    di->ddp = data_read_id(di->ddp, &di->kv.id);
	  di->kv.str = (char *)di->ddp;
	  if (di->nkeynames && di->nparents - di->rootlevel < di->nkeynames)
	    goto di_entersub;
	  if ((di->flags & SEARCH_SUB) != 0)
	    di->state = di_entersub;
	  else
	    di->state = di_nextarrayelement;
	  break;

	case di_entersub: di_entersub:
	  if (di->nparents == sizeof(di->parents)/sizeof(*di->parents) - 1)
	    goto di_nextarrayelement;	/* sorry, full */
	  di->parents[di->nparents].kv = di->kv;
	  di->parents[di->nparents].dp = di->dp;
	  di->parents[di->nparents].keyp = di->keyp;
	  di->dp = (unsigned char *)di->kv.str;
	  di->keyp = di->data->schemadata + di->data->schemata[di->kv.id];
	  memset(&di->kv, 0, sizeof(di->kv));
	  di->kv.parent = &di->parents[di->nparents].kv;
	  di->nparents++;
	  di->keyname = di->keynames[di->nparents - di->rootlevel];
	  goto di_enterschema;

	case di_leavesub: di_leavesub:
	  if (di->nparents - 1 < di->rootlevel)
	    goto di_bye;
	  di->nparents--;
	  di->dp = di->parents[di->nparents].dp;
	  di->kv = di->parents[di->nparents].kv;
	  di->keyp = di->parents[di->nparents].keyp;
	  di->key = di->data->keys + *di->keyp;
	  di->ddp = (unsigned char *)di->kv.str;
	  di->keyname = di->keynames[di->nparents - di->rootlevel];
	  goto di_nextarrayelement;

        /* special solvable attr handling follows */

	case di_nextsolvablekey: di_nextsolvablekey:
	  if (di->keyname)
	    goto di_nextsolvable;
	  if (di->key->name == RPM_RPMDBID)	/* reached end of list? */
	    goto di_leavesolvablekey;
	  di->key++;
	  /* FALLTHROUGH */

	case di_entersolvablekey: di_entersolvablekey:
	  di->idp = solvabledata_fetch(di->pool->solvables + di->solvid, &di->kv, di->key->name);
	  if (!di->idp || !*di->idp)
	    goto di_nextsolvablekey;
	  if (di->kv.eof)
	    {
	      /* not an array */
	      di->kv.id = *di->idp;
	      di->kv.num = *di->idp;	/* for rpmdbid */
	      di->kv.num2 = 0;		/* for rpmdbid */
	      di->kv.entry = 0;
	      di->state = di_nextsolvablekey;
	      break;
	    }
	  di->kv.entry = -1;
	  /* FALLTHROUGH */

	case di_nextsolvableattr:
	  di->state = di_nextsolvableattr;
	  di->kv.id = *di->idp++;
	  di->kv.entry++;
	  if (!*di->idp)
	    {
	      di->kv.eof = 1;
	      di->state = di_nextsolvablekey;
	    }
	  break;

	}

      /* we have a potential match */
      if (di->matcher.match)
	{
	  const char *str;
	  /* simple pre-check so that we don't need to stringify */
	  if (di->keyname == SOLVABLE_FILELIST && di->key->type == REPOKEY_TYPE_DIRSTRARRAY && (di->matcher.flags & SEARCH_FILES) != 0)
	    if (!datamatcher_checkbasename(&di->matcher, di->kv.str))
	      continue;
	  /* now stringify so that we can do the matching */
	  if (!(str = repodata_stringify(di->pool, di->data, di->key, &di->kv, di->flags)))
	    {
	      if (di->keyname && (di->key->type == REPOKEY_TYPE_FIXARRAY || di->key->type == REPOKEY_TYPE_FLEXARRAY))
		return 1;
	      continue;
	    }
	  if (!datamatcher_match(&di->matcher, str))
	    continue;
	}
      else
	{
	  /* stringify filelist if requested */
	  if (di->keyname == SOLVABLE_FILELIST && di->key->type == REPOKEY_TYPE_DIRSTRARRAY && (di->flags & SEARCH_FILES) != 0)
	    repodata_stringify(di->pool, di->data, di->key, &di->kv, di->flags);
	}
      /* found something! */
      return 1;
    }
}