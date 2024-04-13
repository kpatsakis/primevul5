ftp_get_listing (struct url *u, ccon *con, struct fileinfo **f)
{
  uerr_t err;
  char *uf;                     /* url file name */
  char *lf;                     /* list file name */
  char *old_target = con->target;

  con->st &= ~ON_YOUR_OWN;
  con->cmd |= (DO_LIST | LEAVE_PENDING);
  con->cmd &= ~DO_RETR;

  /* Find the listing file name.  We do it by taking the file name of
     the URL and replacing the last component with the listing file
     name.  */
  uf = url_file_name (u, NULL);
  lf = file_merge (uf, LIST_FILENAME);
  xfree (uf);
  DEBUGP ((_("Using %s as listing tmp file.\n"), quote (lf)));

  con->target = xstrdup (lf);
  xfree (lf);
  err = ftp_loop_internal (u, NULL, con, NULL, false);
  lf = xstrdup (con->target);
  xfree (con->target);
  con->target = old_target;

  if (err == RETROK)
    {
      *f = ftp_parse_ls (lf, con->rs);
      if (opt.remove_listing)
        {
          if (unlink (lf))
            logprintf (LOG_NOTQUIET, "unlink: %s\n", strerror (errno));
          else
            logprintf (LOG_VERBOSE, _("Removed %s.\n"), quote (lf));
        }
    }
  else
    *f = NULL;
  xfree (lf);
  con->cmd &= ~DO_LIST;
  return err;
}
