cleanup_redirects (list)
     REDIRECT *list;
{
  do_redirections (list, RX_ACTIVE);
  dispose_redirects (list);
}