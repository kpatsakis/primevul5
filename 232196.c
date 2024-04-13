static int sst_flush_tables(THD* thd)
{
  WSREP_INFO("Flushing tables for SST...");

  int err;
  int not_used;
  if (run_sql_command(thd, "FLUSH TABLES WITH READ LOCK"))
  {
    WSREP_ERROR("Failed to flush and lock tables");
    err = -1;
  }
  else
  {
    /* make sure logs are flushed after global read lock acquired */
    err= reload_acl_and_cache(thd, REFRESH_ENGINE_LOG | REFRESH_BINARY_LOG,
			      (TABLE_LIST*) 0, &not_used);
  }

  if (err)
  {
    WSREP_ERROR("Failed to flush tables: %d (%s)", err, strerror(err));
  }
  else
  {
    WSREP_INFO("Tables flushed.");
    const char base_name[]= "tables_flushed";
    ssize_t const full_len= strlen(mysql_real_data_home) + strlen(base_name)+2;
    char *real_name = (char*) malloc(full_len);
    sprintf(real_name, "%s/%s", mysql_real_data_home, base_name);
    char *tmp_name = (char*) malloc(full_len + 4);
    sprintf(tmp_name, "%s.tmp", real_name);

    FILE* file= fopen(tmp_name, "w+");
    if (0 == file)
    {
      err= errno;
      WSREP_ERROR("Failed to open '%s': %d (%s)", tmp_name, err,strerror(err));
    }
    else
    {
      fprintf(file, "%s:%lld\n",
              wsrep_cluster_state_uuid, (long long)wsrep_locked_seqno);
      fsync(fileno(file));
      fclose(file);
      if (rename(tmp_name, real_name) == -1)
      {
        err= errno;
        WSREP_ERROR("Failed to rename '%s' to '%s': %d (%s)",
                     tmp_name, real_name, err,strerror(err));
      }
    }
    free(real_name);
    free(tmp_name);
  }

  return err;
}