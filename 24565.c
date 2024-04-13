static handler *maria_create_handler(handlerton *hton,
                                     TABLE_SHARE * table,
                                     MEM_ROOT *mem_root)
{
  return new (mem_root) ha_maria(hton, table);
}