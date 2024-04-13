my_bool ma_killed_in_mariadb(MARIA_HA *info)
{
  return (((TABLE*) (info->external_ref))->in_use->killed != 0);
}