file_extension(const char *s)	/* I - Filename or URL */
{
  const char	*extension;	/* Pointer to directory separator */
  char		*bufptr;	/* Pointer into buffer */
  static char	buf[1024];	/* Buffer for files with targets */


  if (s == NULL)
    return (NULL);
  else if (!strncmp(s, "data:image/bmp;", 15))
    return ("bmp");
  else if (!strncmp(s, "data:image/gif;", 15))
    return ("gif");
  else if (!strncmp(s, "data:image/jpeg;", 16))
    return ("jpg");
  else if (!strncmp(s, "data:image/png;", 15))
    return ("png");
  else if ((extension = strrchr(s, '/')) != NULL)
    extension ++;
  else if ((extension = strrchr(s, '\\')) != NULL)
    extension ++;
  else
    extension = s;

  if ((extension = strrchr(extension, '.')) == NULL)
    return ("");
  else
    extension ++;

  if (strchr(extension, '#') == NULL)
    return (extension);

  strlcpy(buf, extension, sizeof(buf));

  if ((bufptr = strchr(buf, '#')) != NULL)
    *bufptr = '\0';

  return (buf);
}