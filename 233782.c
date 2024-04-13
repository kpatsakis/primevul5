clean_up_stdout (void)
{
  if (! write_error_seen)
    close_stdout ();
}