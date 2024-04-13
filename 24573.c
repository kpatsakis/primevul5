int maria_checkpoint_state(handlerton *hton, bool disabled)
{
  maria_checkpoint_disabled= (my_bool) disabled;
  return 0;
}