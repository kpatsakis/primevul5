sh_get_home_dir ()
{
  if (current_user.home_dir == 0)
    get_current_user_info ();
  return current_user.home_dir;
}