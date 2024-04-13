repodata_serialize_key(Repodata *data, struct extdata *newincore,
		       struct extdata *newvincore,
		       Id *schema,
		       Repokey *key, Id val)
{
  Id *ida;
  struct extdata *xd;
  unsigned int oldvincorelen = 0;
  Id schemaid, *sp;

  xd = newincore;
  if (key->storage == KEY_STORAGE_VERTICAL_OFFSET)
    {
      xd = newvincore;
      oldvincorelen = xd->len;
    }
  switch (key->type)
    {
    case REPOKEY_TYPE_VOID:
    case REPOKEY_TYPE_CONSTANT:
    case REPOKEY_TYPE_CONSTANTID:
    case REPOKEY_TYPE_DELETED:
      break;
    case REPOKEY_TYPE_STR:
      data_addblob(xd, data->attrdata + val, strlen((char *)(data->attrdata + val)) + 1);
      break;
    case REPOKEY_TYPE_MD5:
      data_addblob(xd, data->attrdata + val, SIZEOF_MD5);
      break;
    case REPOKEY_TYPE_SHA1:
      data_addblob(xd, data->attrdata + val, SIZEOF_SHA1);
      break;
    case REPOKEY_TYPE_SHA224:
      data_addblob(xd, data->attrdata + val, SIZEOF_SHA224);
      break;
    case REPOKEY_TYPE_SHA256:
      data_addblob(xd, data->attrdata + val, SIZEOF_SHA256);
      break;
    case REPOKEY_TYPE_SHA384:
      data_addblob(xd, data->attrdata + val, SIZEOF_SHA384);
      break;
    case REPOKEY_TYPE_SHA512:
      data_addblob(xd, data->attrdata + val, SIZEOF_SHA512);
      break;
    case REPOKEY_TYPE_NUM:
      if (val & 0x80000000)
	{
	  data_addid64(xd, data->attrnum64data[val ^ 0x80000000]);
	  break;
	}
      /* FALLTHROUGH */
    case REPOKEY_TYPE_ID:
    case REPOKEY_TYPE_DIR:
      data_addid(xd, val);
      break;
    case REPOKEY_TYPE_BINARY:
      {
	Id len;
	unsigned char *dp = data_read_id(data->attrdata + val, &len);
	dp += (unsigned int)len;
	data_addblob(xd, data->attrdata + val, dp - (data->attrdata + val));
      }
      break;
    case REPOKEY_TYPE_IDARRAY:
      for (ida = data->attriddata + val; *ida; ida++)
	data_addideof(xd, ida[0], ida[1] ? 0 : 1);
      break;
    case REPOKEY_TYPE_DIRNUMNUMARRAY:
      for (ida = data->attriddata + val; *ida; ida += 3)
	{
	  data_addid(xd, ida[0]);
	  data_addid(xd, ida[1]);
	  data_addideof(xd, ida[2], ida[3] ? 0 : 1);
	}
      break;
    case REPOKEY_TYPE_DIRSTRARRAY:
      for (ida = data->attriddata + val; *ida; ida += 2)
	{
	  data_addideof(xd, ida[0], ida[2] ? 0 : 1);
	  data_addblob(xd, data->attrdata + ida[1], strlen((char *)(data->attrdata + ida[1])) + 1);
	}
      break;
    case REPOKEY_TYPE_FIXARRAY:
      {
	int num = 0;
	schemaid = 0;
	for (ida = data->attriddata + val; *ida; ida++)
	  {
	    Id *kp;
	    sp = schema;
	    kp = data->xattrs[-*ida];
	    if (!kp)
	      continue;		/* ignore empty elements */
	    num++;
	    for (; *kp; kp += 2)
	      *sp++ = *kp;
	    *sp = 0;
	    if (!schemaid)
	      schemaid = repodata_schema2id(data, schema, 1);
	    else if (schemaid != repodata_schema2id(data, schema, 0))
	      {
	 	pool_debug(data->repo->pool, SOLV_ERROR, "repodata_serialize_key: fixarray substructs with different schemas\n");
		num = 0;
		break;
	      }
	  }
	data_addid(xd, num);
	if (!num)
	  break;
	data_addid(xd, schemaid);
	for (ida = data->attriddata + val; *ida; ida++)
	  {
	    Id *kp = data->xattrs[-*ida];
	    if (!kp)
	      continue;
	    for (; *kp; kp += 2)
	      repodata_serialize_key(data, newincore, newvincore, schema, data->keys + *kp, kp[1]);
	  }
	break;
      }
    case REPOKEY_TYPE_FLEXARRAY:
      {
	int num = 0;
	for (ida = data->attriddata + val; *ida; ida++)
	  num++;
	data_addid(xd, num);
	for (ida = data->attriddata + val; *ida; ida++)
	  {
	    Id *kp = data->xattrs[-*ida];
	    if (!kp)
	      {
	        data_addid(xd, 0);	/* XXX */
	        continue;
	      }
	    sp = schema;
	    for (;*kp; kp += 2)
	      *sp++ = *kp;
	    *sp = 0;
	    schemaid = repodata_schema2id(data, schema, 1);
	    data_addid(xd, schemaid);
	    kp = data->xattrs[-*ida];
	    for (;*kp; kp += 2)
	      repodata_serialize_key(data, newincore, newvincore, schema, data->keys + *kp, kp[1]);
	  }
	break;
      }
    default:
      pool_debug(data->repo->pool, SOLV_FATAL, "repodata_serialize_key: don't know how to handle type %d\n", key->type);
      exit(1);
    }
  if (key->storage == KEY_STORAGE_VERTICAL_OFFSET)
    {
      /* put offset/len in incore */
      data_addid(newincore, data->lastverticaloffset + oldvincorelen);
      oldvincorelen = xd->len - oldvincorelen;
      data_addid(newincore, oldvincorelen);
    }
}