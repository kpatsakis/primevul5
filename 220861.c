data_fetch_uninternalized(Repodata *data, Repokey *key, Id value, KeyValue *kv)
{
  Id *array;
  kv->eof = 1;
  switch (key->type)
    {
    case REPOKEY_TYPE_STR:
      kv->str = (const char *)data->attrdata + value;
      return;
    case REPOKEY_TYPE_CONSTANT:
      kv->num2 = 0;
      kv->num = key->size;
      return;
    case REPOKEY_TYPE_CONSTANTID:
      kv->id = key->size;
      return;
    case REPOKEY_TYPE_NUM:
      kv->num2 = 0;
      kv->num = value;
      if (value & 0x80000000)
	{
	  kv->num = (unsigned int)data->attrnum64data[value ^ 0x80000000];
	  kv->num2 = (unsigned int)(data->attrnum64data[value ^ 0x80000000] >> 32);
	}
      return;
    case_CHKSUM_TYPES:
      kv->num = 0;	/* not stringified */
      kv->str = (const char *)data->attrdata + value;
      return;
    case REPOKEY_TYPE_BINARY:
      kv->str = (const char *)data_read_id(data->attrdata + value, (Id *)&kv->num);
      return;
    case REPOKEY_TYPE_IDARRAY:
      array = data->attriddata + (value + kv->entry);
      kv->id = array[0];
      kv->eof = array[1] ? 0 : 1;
      return;
    case REPOKEY_TYPE_DIRSTRARRAY:
      kv->num = 0;	/* not stringified */
      array = data->attriddata + (value + kv->entry * 2);
      kv->id = array[0];
      kv->str = (const char *)data->attrdata + array[1];
      kv->eof = array[2] ? 0 : 1;
      return;
    case REPOKEY_TYPE_DIRNUMNUMARRAY:
      array = data->attriddata + (value + kv->entry * 3);
      kv->id = array[0];
      kv->num = array[1];
      kv->num2 = array[2];
      kv->eof = array[3] ? 0 : 1;
      return;
    case REPOKEY_TYPE_FIXARRAY:
    case REPOKEY_TYPE_FLEXARRAY:
      array = data->attriddata + (value + kv->entry);
      kv->id = array[0];		/* the handle */
      kv->eof = array[1] ? 0 : 1;
      return;
    default:
      kv->id = value;
      return;
    }
}