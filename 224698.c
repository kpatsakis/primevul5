restore_stdin (s)
     int s;
{
  dup2 (s, 0);
  close (s);
}