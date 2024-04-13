int pidfile_set(const char *pidfile)
{
   g_pidfile = strdup(pidfile);
   if (!g_pidfile) {
       logprintf(STDERR_FILENO, "Out of memory.\n");
       return -1;
   }

   return 0;
}