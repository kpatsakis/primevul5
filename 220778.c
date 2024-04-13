data_skip_schema(Repodata *data, unsigned char *dp, Id schema)
{
  Id *keyp = data->schemadata + data->schemata[schema];
  for (; *keyp; keyp++)
    dp = data_skip_key(data, dp, data->keys + *keyp);
  return dp;
}