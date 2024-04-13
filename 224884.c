execute_shell_function (var, words)
     SHELL_VAR *var;
     WORD_LIST *words;
{
  int ret;
  struct fd_bitmap *bitmap;

  bitmap = new_fd_bitmap (FD_BITMAP_DEFAULT_SIZE);
  begin_unwind_frame ("execute-shell-function");
  add_unwind_protect (dispose_fd_bitmap, (char *)bitmap);
      
  ret = execute_function (var, words, 0, bitmap, 0, 0);

  dispose_fd_bitmap (bitmap);
  discard_unwind_frame ("execute-shell-function");

  return ret;
}