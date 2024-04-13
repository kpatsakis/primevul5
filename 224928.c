close_pipes (in, out)
     int in, out;
{
  if (in >= 0)
    close (in);
  if (out >= 0)
    close (out);
}