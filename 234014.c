append_dir_structure (const struct url *u, struct growable *dest)
{
  char *pathel, *next;
  int cut = opt.cut_dirs;

  /* Go through the path components, de-URL-quote them, and quote them
     (if necessary) as file names.  */

  pathel = u->path;
  for (; (next = strchr (pathel, '/')) != NULL; pathel = next + 1)
    {
      if (cut-- > 0)
        continue;
      if (pathel == next)
        /* Ignore empty pathels.  */
        continue;

      if (dest->tail)
        append_char ('/', dest);
      append_uri_pathel (pathel, next, true, dest);
    }
}