void Regexp_processor_pcre::fix_owner(Item_func *owner,
                                      Item *subject_arg,
                                      Item *pattern_arg)
{
  if (!is_compiled() && pattern_arg->const_item())
  {
    if (compile(pattern_arg, true))
    {
      owner->maybe_null= 1; // Will always return NULL
      return;
    }
    set_const(true);
    owner->maybe_null= subject_arg->maybe_null;
  }
  else
    owner->maybe_null= 1;
}