my_bool mysql_rm_tmp_tables(void)
{
  uint i, idx;
  char	filePath[FN_REFLEN], *tmpdir, filePathCopy[FN_REFLEN];
  MY_DIR *dirp;
  FILEINFO *file;
  TABLE_SHARE share;
  THD *thd;
  DBUG_ENTER("mysql_rm_tmp_tables");

  if (!(thd= new THD(0)))
    DBUG_RETURN(1);
  thd->thread_stack= (char*) &thd;
  thd->store_globals();

  for (i=0; i<=mysql_tmpdir_list.max; i++)
  {
    tmpdir=mysql_tmpdir_list.list[i];
    /* See if the directory exists */
    if (!(dirp = my_dir(tmpdir,MYF(MY_WME | MY_DONT_SORT))))
      continue;

    /* Remove all SQLxxx tables from directory */

    for (idx=0 ; idx < (uint) dirp->number_of_files ; idx++)
    {
      file=dirp->dir_entry+idx;

      if (!strncmp(file->name, tmp_file_prefix, tmp_file_prefix_length))
      {
        char *ext= fn_ext(file->name);
        uint ext_len= strlen(ext);
        uint filePath_len= my_snprintf(filePath, sizeof(filePath),
                                       "%s%c%s", tmpdir, FN_LIBCHAR,
                                       file->name);
        if (!strcmp(reg_ext, ext))
        {
          handler *handler_file= 0;
          /* We should cut file extention before deleting of table */
          memcpy(filePathCopy, filePath, filePath_len - ext_len);
          filePathCopy[filePath_len - ext_len]= 0;
          init_tmp_table_share(thd, &share, "", 0, "", filePathCopy);
          if (!open_table_def(thd, &share) &&
              ((handler_file= get_new_handler(&share, thd->mem_root,
                                              share.db_type()))))
          {
            handler_file->ha_delete_table(filePathCopy);
            delete handler_file;
          }
          free_table_share(&share);
        }
        /*
          File can be already deleted by tmp_table.file->delete_table().
          So we hide error messages which happnes during deleting of these
          files(MYF(0)).
        */
        (void) mysql_file_delete(key_file_misc, filePath, MYF(0));
      }
    }
    my_dirend(dirp);
  }
  delete thd;
  DBUG_RETURN(0);
}