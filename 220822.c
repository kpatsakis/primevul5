repodata_set_kv(Repodata *data, Id solvid, Id keyname, Id keytype, KeyValue *kv)
{
  switch (keytype)
    {
    case REPOKEY_TYPE_ID:
      repodata_set_id(data, solvid, keyname, kv->id);
      break;
    case REPOKEY_TYPE_CONSTANTID:
      repodata_set_constantid(data, solvid, keyname, kv->id);
      break;
    case REPOKEY_TYPE_IDARRAY:
      repodata_add_idarray(data, solvid, keyname, kv->id);
      break;
    case REPOKEY_TYPE_STR:
      repodata_set_str(data, solvid, keyname, kv->str);
      break;
    case REPOKEY_TYPE_VOID:
      repodata_set_void(data, solvid, keyname);
      break;
    case REPOKEY_TYPE_NUM:
      repodata_set_num(data, solvid, keyname, SOLV_KV_NUM64(kv));
      break;
    case REPOKEY_TYPE_CONSTANT:
      repodata_set_constant(data, solvid, keyname, kv->num);
      break;
    case REPOKEY_TYPE_DIRNUMNUMARRAY:
      if (kv->id)
        repodata_add_dirnumnum(data, solvid, keyname, kv->id, kv->num, kv->num2);
      break;
    case REPOKEY_TYPE_DIRSTRARRAY:
      repodata_add_dirstr(data, solvid, keyname, kv->id, kv->str);
      break;
    case_CHKSUM_TYPES:
      repodata_set_bin_checksum(data, solvid, keyname, keytype, (const unsigned char *)kv->str);
      break;
    default:
      break;
    }
}