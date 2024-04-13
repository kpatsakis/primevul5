path_simplify (enum url_scheme scheme, char *path)
{
  char *h = path;               /* hare */
  char *t = path;               /* tortoise */
  char *beg = path;
  char *end = strchr (path, '\0');

  while (h < end)
    {
      /* Hare should be at the beginning of a path element. */

      if (h[0] == '.' && (h[1] == '/' || h[1] == '\0'))
        {
          /* Ignore "./". */
          h += 2;
        }
      else if (h[0] == '.' && h[1] == '.' && (h[2] == '/' || h[2] == '\0'))
        {
          /* Handle "../" by retreating the tortoise by one path
             element -- but not past beginning.  */
          if (t > beg)
            {
              /* Move backwards until T hits the beginning of the
                 previous path element or the beginning of path. */
              for (--t; t > beg && t[-1] != '/'; t--)
                ;
            }
          else if (scheme == SCHEME_FTP
#ifdef HAVE_SSL
              || scheme == SCHEME_FTPS
#endif
              )
            {
              /* If we're at the beginning, copy the "../" literally
                 and move the beginning so a later ".." doesn't remove
                 it.  This violates RFC 3986; but we do it for FTP
                 anyway because there is otherwise no way to get at a
                 parent directory, when the FTP server drops us in a
                 non-root directory (which is not uncommon). */
              beg = t + 3;
              goto regular;
            }
          h += 3;
        }
      else
        {
        regular:
          /* A regular path element.  If H hasn't advanced past T,
             simply skip to the next path element.  Otherwise, copy
             the path element until the next slash.  */
          if (t == h)
            {
              /* Skip the path element, including the slash.  */
              while (h < end && *h != '/')
                t++, h++;
              if (h < end)
                t++, h++;
            }
          else
            {
              /* Copy the path element, including the final slash.  */
              while (h < end && *h != '/')
                *t++ = *h++;
              if (h < end)
                *t++ = *h++;
            }
        }
    }

  if (t != h)
    *t = '\0';

  return t != h;
}