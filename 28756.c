ftp_retrieve_list (struct url *u, struct fileinfo *f, ccon *con)
{
  static int depth = 0;
  uerr_t err;
  struct fileinfo *orig;
  wgint local_size;
  time_t tml;
  bool dlthis; /* Download this (file). */
  const char *actual_target = NULL;
  bool force_full_retrieve = false;

  /* Increase the depth.  */
  ++depth;
  if (opt.reclevel != INFINITE_RECURSION && depth > opt.reclevel)
    {
      DEBUGP ((_("Recursion depth %d exceeded max. depth %d.\n"),
               depth, opt.reclevel));
      --depth;
      return RECLEVELEXC;
    }

  assert (f != NULL);
  orig = f;

  con->st &= ~ON_YOUR_OWN;
  if (!(con->st & DONE_CWD))
    con->cmd |= DO_CWD;
  else
    con->cmd &= ~DO_CWD;
  con->cmd |= (DO_RETR | LEAVE_PENDING);

  if (con->csock < 0)
    con->cmd |= DO_LOGIN;
  else
    con->cmd &= ~DO_LOGIN;

  err = RETROK;                 /* in case it's not used */

  while (f)
    {
      char *old_target, *ofile;

      if (opt.quota && total_downloaded_bytes > opt.quota)
        {
          --depth;
          return QUOTEXC;
        }
      old_target = con->target;

      ofile = xstrdup (u->file);
      url_set_file (u, f->name);

      con->target = url_file_name (u, NULL);
      err = RETROK;

      dlthis = true;
      if (opt.timestamping && f->type == FT_PLAINFILE)
        {
          struct_stat st;
          /* If conversion of HTML files retrieved via FTP is ever implemented,
             we'll need to stat() <file>.orig here when -K has been specified.
             I'm not implementing it now since files on an FTP server are much
             more likely than files on an HTTP server to legitimately have a
             .orig suffix. */
          if (!stat (con->target, &st))
            {
              bool eq_size;
              bool cor_val;
              /* Else, get it from the file.  */
              local_size = st.st_size;
              tml = st.st_mtime;
#ifdef WINDOWS
              /* Modification time granularity is 2 seconds for Windows, so
                 increase local time by 1 second for later comparison. */
              tml++;
#endif
              /* Compare file sizes only for servers that tell us correct
                 values. Assume sizes being equal for servers that lie
                 about file size.  */
              cor_val = (con->rs == ST_UNIX || con->rs == ST_WINNT);
              eq_size = cor_val ? (local_size == f->size) : true;
              if (f->tstamp <= tml && eq_size)
                {
                  /* Remote file is older, file sizes can be compared and
                     are both equal. */
                  logprintf (LOG_VERBOSE, _("\
Remote file no newer than local file %s -- not retrieving.\n"), quote (con->target));
                  dlthis = false;
                }
              else if (f->tstamp > tml)
                {
                  /* Remote file is newer */
                  force_full_retrieve = true;
                  logprintf (LOG_VERBOSE, _("\
Remote file is newer than local file %s -- retrieving.\n\n"),
                             quote (con->target));
                }
              else
                {
                  /* Sizes do not match */
                  logprintf (LOG_VERBOSE, _("\
The sizes do not match (local %s) -- retrieving.\n\n"),
                             number_to_static_string (local_size));
                }
            }
        }       /* opt.timestamping && f->type == FT_PLAINFILE */
      switch (f->type)
        {
        case FT_SYMLINK:
          /* If opt.retr_symlinks is defined, we treat symlinks as
             if they were normal files.  There is currently no way
             to distinguish whether they might be directories, and
             follow them.  */
          if (!opt.retr_symlinks)
            {
#ifdef HAVE_SYMLINK
              if (!f->linkto)
                logputs (LOG_NOTQUIET,
                         _("Invalid name of the symlink, skipping.\n"));
              else
                {
                  struct_stat st;
                  /* Check whether we already have the correct
                     symbolic link.  */
                  int rc = lstat (con->target, &st);
                  if (rc == 0)
                    {
                      size_t len = strlen (f->linkto) + 1;
                      if (S_ISLNK (st.st_mode))
                        {
                          char *link_target = (char *)alloca (len);
                          size_t n = readlink (con->target, link_target, len);
                          if ((n == len - 1)
                              && (memcmp (link_target, f->linkto, n) == 0))
                            {
                              logprintf (LOG_VERBOSE, _("\
Already have correct symlink %s -> %s\n\n"),
                                         quote (con->target),
                                         quote (f->linkto));
                              dlthis = false;
                              break;
                            }
                        }
                    }
                  logprintf (LOG_VERBOSE, _("Creating symlink %s -> %s\n"),
                             quote (con->target), quote (f->linkto));
                  /* Unlink before creating symlink!  */
                  unlink (con->target);
                  if (symlink (f->linkto, con->target) == -1)
                    logprintf (LOG_NOTQUIET, "symlink: %s\n", strerror (errno));
                  logputs (LOG_VERBOSE, "\n");
                } /* have f->linkto */
#else  /* not HAVE_SYMLINK */
              logprintf (LOG_NOTQUIET,
                         _("Symlinks not supported, skipping symlink %s.\n"),
                         quote (con->target));
#endif /* not HAVE_SYMLINK */
            }
          else                /* opt.retr_symlinks */
            {
              if (dlthis)
                err = ftp_loop_internal (u, f, con, NULL, force_full_retrieve);
            } /* opt.retr_symlinks */
          break;
        case FT_DIRECTORY:
          if (!opt.recursive)
            logprintf (LOG_NOTQUIET, _("Skipping directory %s.\n"),
                       quote (f->name));
          break;
        case FT_PLAINFILE:
          /* Call the retrieve loop.  */
          if (dlthis)
            err = ftp_loop_internal (u, f, con, NULL, force_full_retrieve);
          break;
        case FT_UNKNOWN:
          logprintf (LOG_NOTQUIET, _("%s: unknown/unsupported file type.\n"),
                     quote (f->name));
          break;
        }       /* switch */


      /* 2004-12-15 SMS.
       * Set permissions _before_ setting the times, as setting the
       * permissions changes the modified-time, at least on VMS.
       * Also, use the opt.output_document name here, too, as
       * appropriate.  (Do the test once, and save the result.)
       */

      set_local_file (&actual_target, con->target);

      /* If downloading a plain file, and the user requested it, then
         set valid (non-zero) permissions. */
      if (dlthis && (actual_target != NULL) &&
       (f->type == FT_PLAINFILE) && opt.preserve_perm)
        {
          if (f->perms)
            chmod (actual_target, f->perms);
          else
            DEBUGP (("Unrecognized permissions for %s.\n", actual_target));
        }

      /* Set the time-stamp information to the local file.  Symlinks
         are not to be stamped because it sets the stamp on the
         original.  :( */
      if (actual_target != NULL)
        {
          if (opt.useservertimestamps
              && !(f->type == FT_SYMLINK && !opt.retr_symlinks)
              && f->tstamp != -1
              && dlthis
              && file_exists_p (con->target))
            {
              touch (actual_target, f->tstamp);
            }
          else if (f->tstamp == -1)
            logprintf (LOG_NOTQUIET, _("%s: corrupt time-stamp.\n"),
                       actual_target);
        }

      xfree (con->target);
      con->target = old_target;

      url_set_file (u, ofile);
      xfree (ofile);

      /* Break on fatals.  */
      if (err == QUOTEXC || err == HOSTERR || err == FWRITEERR
          || err == WARC_ERR || err == WARC_TMP_FOPENERR
          || err == WARC_TMP_FWRITEERR)
        break;
      con->cmd &= ~ (DO_CWD | DO_LOGIN);
      f = f->next;
    }

  /* We do not want to call ftp_retrieve_dirs here */
  if (opt.recursive &&
      !(opt.reclevel != INFINITE_RECURSION && depth >= opt.reclevel))
    err = ftp_retrieve_dirs (u, orig, con);
  else if (opt.recursive)
    DEBUGP ((_("Will not retrieve dirs since depth is %d (max %d).\n"),
             depth, opt.reclevel));
  --depth;
  return err;
}
