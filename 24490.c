static my_bool translog_callback_delete_all(const char *directory,
                                            const char *filename)
{
  char complete_name[FN_REFLEN];
  fn_format(complete_name, filename, directory, "", MYF(MY_UNPACK_FILENAME));
  return mysql_file_delete(key_file_translog, complete_name, MYF(MY_WME));
}