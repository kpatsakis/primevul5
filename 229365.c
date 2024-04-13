static MARIADB_CONST_STRING ma_const_string_copy_root(MA_MEM_ROOT *memroot,
                                                      const char *str,
                                                      size_t length)
{
  MARIADB_CONST_STRING res;
  if (!str || !(res.str= ma_memdup_root(memroot, str, length)))
    return null_const_string;
  res.length= length;
  return res;
}