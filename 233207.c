static int file_lookup(struct host_query *hquery)
{
  FILE *fp;
  int error;
  int status;
  const char *path_hosts = NULL;

  if (hquery->hints.ai_flags & ARES_AI_ENVHOSTS)
    {
      path_hosts = getenv("CARES_HOSTS");
    }

  if (!path_hosts)
    {
#ifdef WIN32
      char PATH_HOSTS[MAX_PATH];
      win_platform platform;

      PATH_HOSTS[0] = '\0';

      platform = ares__getplatform();

      if (platform == WIN_NT)
        {
          char tmp[MAX_PATH];
          HKEY hkeyHosts;

          if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, WIN_NS_NT_KEY, 0, KEY_READ,
                           &hkeyHosts) == ERROR_SUCCESS)
            {
              DWORD dwLength = MAX_PATH;
              RegQueryValueExA(hkeyHosts, DATABASEPATH, NULL, NULL, (LPBYTE)tmp,
                              &dwLength);
              ExpandEnvironmentStringsA(tmp, PATH_HOSTS, MAX_PATH);
              RegCloseKey(hkeyHosts);
            }
        }
      else if (platform == WIN_9X)
        GetWindowsDirectoryA(PATH_HOSTS, MAX_PATH);
      else
        return ARES_ENOTFOUND;

      strcat(PATH_HOSTS, WIN_PATH_HOSTS);
      path_hosts = PATH_HOSTS;

#elif defined(WATT32)
      const char *PATH_HOSTS = _w32_GetHostsFile();

      if (!PATH_HOSTS)
        return ARES_ENOTFOUND;
#endif
      path_hosts = PATH_HOSTS;
    }

  fp = fopen(path_hosts, "r");
  if (!fp)
    {
      error = ERRNO;
      switch (error)
        {
        case ENOENT:
        case ESRCH:
          return ARES_ENOTFOUND;
        default:
          DEBUGF(fprintf(stderr, "fopen() failed with error: %d %s\n", error,
                         strerror(error)));
          DEBUGF(fprintf(stderr, "Error opening file: %s\n", path_hosts));
          return ARES_EFILE;
        }
    }
  status = ares__readaddrinfo(fp, hquery->name, hquery->port, &hquery->hints, hquery->ai);
  fclose(fp);
  return status;
}