grepdir (char const *dir, struct stats const *stats)
{
  char const *dir_or_dot = (dir ? dir : ".");
  struct stats const *ancestor;
  char *name_space;
  int status = 1;
  if (excluded_directory_patterns
      && excluded_file_name (excluded_directory_patterns, dir))
    return 1;

  /* Mingw32 does not support st_ino.  No known working hosts use zero
     for st_ino, so assume that the Mingw32 bug applies if it's zero.  */
  if (stats->stat.st_ino)
    {
      for (ancestor = stats; (ancestor = ancestor->parent) != 0;  )
        {
          if (ancestor->stat.st_ino == stats->stat.st_ino
              && ancestor->stat.st_dev == stats->stat.st_dev)
            {
              if (!suppress_errors)
                error (0, 0, _("warning: %s: %s"), dir,
                       _("recursive directory loop"));
              errseen = 1;
              return 1;
            }
        }
    }

  name_space = savedir (dir_or_dot, stats->stat.st_size, included_patterns,
                        excluded_patterns, excluded_directory_patterns);

  if (! name_space)
    {
      if (errno)
        suppressible_error (dir_or_dot, errno);
      else
        xalloc_die ();
    }
  else
    {
      size_t dirlen = 0;
      int needs_slash = 0;
      char *file_space = NULL;
      char const *namep = name_space;
      struct stats child;
      if (dir)
        {
          dirlen = strlen (dir);
          needs_slash = ! (dirlen == FILE_SYSTEM_PREFIX_LEN (dir)
                           || ISSLASH (dir[dirlen - 1]));
        }
      child.parent = stats;
      out_file += !no_filenames;
      while (*namep)
        {
          size_t namelen = strlen (namep);
          char const *file;
          if (! dir)
            file = namep;
          else
            {
              file_space = xrealloc (file_space, dirlen + 1 + namelen + 1);
              strcpy (file_space, dir);
              file_space[dirlen] = '/';
              strcpy (file_space + dirlen + needs_slash, namep);
              file = file_space;
            }
          namep += namelen + 1;
          status &= grepfile (file, &child);
        }
      out_file -= !no_filenames;
      free (file_space);
      free (name_space);
    }

  return status;
}