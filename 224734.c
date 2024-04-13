seedrand ()
{
  struct timeval tv;

  gettimeofday (&tv, NULL);
  sbrand (tv.tv_sec ^ tv.tv_usec ^ getpid ());
}