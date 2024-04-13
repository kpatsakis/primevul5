maybe_load_repodata(Repodata *data, Id keyname)
{
  if (keyname && !repodata_precheck_keyname(data, keyname))
    return 0;	/* do not bother... */
  if (data->state == REPODATA_AVAILABLE || data->state == REPODATA_LOADING)
    return 1;
  if (data->state == REPODATA_ERROR)
    return 0;
  return maybe_load_repodata_stub(data, keyname);
}