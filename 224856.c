uidset ()
{
  char buff[INT_STRLEN_BOUND(uid_t) + 1], *b;
  register SHELL_VAR *v;

  b = inttostr (current_user.uid, buff, sizeof (buff));
  v = find_variable ("UID");
  if (v == 0)
    {
      v = bind_variable ("UID", b, 0);
      VSETATTR (v, (att_readonly | att_integer));
    }

  if (current_user.euid != current_user.uid)
    b = inttostr (current_user.euid, buff, sizeof (buff));

  v = find_variable ("EUID");
  if (v == 0)
    {
      v = bind_variable ("EUID", b, 0);
      VSETATTR (v, (att_readonly | att_integer));
    }
}