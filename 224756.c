dispose_temporary_env (pushf)
     sh_free_func_t *pushf;
{
  int i;

  tempvar_list = strvec_create (HASH_ENTRIES (temporary_env) + 1);
  tempvar_list[tvlist_ind = 0] = 0;
    
  hash_flush (temporary_env, pushf);
  hash_dispose (temporary_env);
  temporary_env = (HASH_TABLE *)NULL;

  tempvar_list[tvlist_ind] = 0;

  array_needs_making = 1;

#if 0
  sv_ifs ("IFS");		/* XXX here for now -- check setifs in assign_in_env */  
#endif
  for (i = 0; i < tvlist_ind; i++)
    stupidly_hack_special_variables (tempvar_list[i]);

  strvec_dispose (tempvar_list);
  tempvar_list = 0;
  tvlist_ind = 0;
}