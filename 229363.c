static size_t mariadb_server_version_id(MYSQL *mysql)
{
  size_t major, minor, patch;
  char *p;

  if (!(p = mysql->server_version)) {
    return 0;
  }

  major = strtol(p, &p, 10);
  p += 1; /* consume the dot */
  minor = strtol(p, &p, 10);
  p += 1; /* consume the dot */
  patch = strtol(p, &p, 10);

  return (major * 10000L + (unsigned long)(minor * 100L + patch));
}