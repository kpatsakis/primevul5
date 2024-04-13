execute_command (command)
     COMMAND *command;
{
  struct fd_bitmap *bitmap;
  int result;

  current_fds_to_close = (struct fd_bitmap *)NULL;
  bitmap = new_fd_bitmap (FD_BITMAP_DEFAULT_SIZE);
  begin_unwind_frame ("execute-command");
  add_unwind_protect (dispose_fd_bitmap, (char *)bitmap);

  /* Just do the command, but not asynchronously. */
  result = execute_command_internal (command, 0, NO_PIPE, NO_PIPE, bitmap);

  dispose_fd_bitmap (bitmap);
  discard_unwind_frame ("execute-command");

#if defined (PROCESS_SUBSTITUTION)
  /* don't unlink fifos if we're in a shell function; wait until the function
     returns. */
  if (variable_context == 0)
    unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */

  QUIT;
  return (result);
}