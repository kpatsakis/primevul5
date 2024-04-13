static st_plugin_dl *plugin_dl_add(const LEX_STRING *dl, int report)
{
#ifdef HAVE_DLOPEN
  char dlpath[FN_REFLEN];
  uint plugin_dir_len, dummy_errors, i;
  struct st_plugin_dl *tmp= 0, plugin_dl;
  void *sym;
  st_ptr_backup tmp_backup[array_elements(list_of_services)];
  DBUG_ENTER("plugin_dl_add");
  DBUG_PRINT("enter", ("dl->str: '%s', dl->length: %d",
                       dl->str, (int) dl->length));
  mysql_mutex_assert_owner(&LOCK_plugin);
  plugin_dir_len= strlen(opt_plugin_dir);
  /*
    Ensure that the dll doesn't have a path.
    This is done to ensure that only approved libraries from the
    plugin directory are used (to make this even remotely secure).
  */
  if (check_string_char_length((LEX_STRING *) dl, 0, NAME_CHAR_LEN,
                               system_charset_info, 1) ||
      check_valid_path(dl->str, dl->length) ||
      plugin_dir_len + dl->length + 1 >= FN_REFLEN)
  {
    report_error(report, ER_UDF_NO_PATHS);
    DBUG_RETURN(0);
  }
  /* If this dll is already loaded just increase ref_count. */
  if ((tmp= plugin_dl_find(dl)))
  {
    tmp->ref_count++;
    DBUG_RETURN(tmp);
  }
  bzero(&plugin_dl, sizeof(plugin_dl));
  /* Compile dll path */
  strxnmov(dlpath, sizeof(dlpath) - 1, opt_plugin_dir, "/", dl->str, NullS);
  (void) unpack_filename(dlpath, dlpath);
  plugin_dl.ref_count= 1;
  /* Open new dll handle */
  if (!(plugin_dl.handle= dlopen(dlpath, RTLD_NOW)))
  {
    report_error(report, ER_CANT_OPEN_LIBRARY, dlpath, errno, my_dlerror(dlpath));
    goto ret;
  }
  dlopen_count++;

#ifdef HAVE_LINK_H
  if (global_system_variables.log_warnings > 2)
  {
    struct link_map *lm = (struct link_map*) plugin_dl.handle;
    sql_print_information("Loaded '%s' with offset 0x%zx", dl->str, (size_t)lm->l_addr);
  }
#endif

  /* Checks which plugin interface present and reads info */
  if (!(sym= dlsym(plugin_dl.handle, maria_plugin_interface_version_sym)))
  {
    if (read_mysql_plugin_info(&plugin_dl,
                               dlsym(plugin_dl.handle,
                                     plugin_interface_version_sym),
                               dlpath,
                               report))
      goto ret;
  }
  else
  {
    if (read_maria_plugin_info(&plugin_dl, sym, dlpath, report))
      goto ret;
  }

  /* link the services in */
  for (i= 0; i < array_elements(list_of_services); i++)
  {
    if ((sym= dlsym(plugin_dl.handle, list_of_services[i].name)))
    {
      void **ptr= (void **)sym;
      uint ver= (uint)(intptr)*ptr;
      if (ver > list_of_services[i].version ||
        (ver >> 8) < (list_of_services[i].version >> 8))
      {
        char buf[MYSQL_ERRMSG_SIZE];
        my_snprintf(buf, sizeof(buf),
                    "service '%s' interface version mismatch",
                    list_of_services[i].name);
        report_error(report, ER_CANT_OPEN_LIBRARY, dlpath, ENOEXEC, buf);
        goto ret;
      }
      tmp_backup[plugin_dl.nbackups++].save(ptr);
      *ptr= list_of_services[i].service;
    }
  }

  if (plugin_dl.nbackups)
  {
    size_t bytes= plugin_dl.nbackups * sizeof(plugin_dl.ptr_backup[0]);
    plugin_dl.ptr_backup= (st_ptr_backup *)my_malloc(bytes, MYF(0));
    if (!plugin_dl.ptr_backup)
    {
      restore_ptr_backup(plugin_dl.nbackups, tmp_backup);
      report_error(report, ER_OUTOFMEMORY, bytes);
      goto ret;
    }
    memcpy(plugin_dl.ptr_backup, tmp_backup, bytes);
  }

  /* Duplicate and convert dll name */
  plugin_dl.dl.length= dl->length * files_charset_info->mbmaxlen + 1;
  if (! (plugin_dl.dl.str= (char*) my_malloc(plugin_dl.dl.length, MYF(0))))
  {
    report_error(report, ER_OUTOFMEMORY,
                 static_cast<int>(plugin_dl.dl.length));
    goto ret;
  }
  plugin_dl.dl.length= copy_and_convert(plugin_dl.dl.str, plugin_dl.dl.length,
    files_charset_info, dl->str, dl->length, system_charset_info,
    &dummy_errors);
  plugin_dl.dl.str[plugin_dl.dl.length]= 0;
  /* Add this dll to array */
  if (! (tmp= plugin_dl_insert_or_reuse(&plugin_dl)))
  {
    report_error(report, ER_OUTOFMEMORY,
                 static_cast<int>(sizeof(struct st_plugin_dl)));
    goto ret;
  }

ret:
  if (!tmp)
    free_plugin_mem(&plugin_dl);

  DBUG_RETURN(tmp);

#else
  DBUG_ENTER("plugin_dl_add");
  report_error(report, ER_FEATURE_DISABLED, "plugin", "HAVE_DLOPEN");
  DBUG_RETURN(0);
#endif
}