initialize_dynamic_variables ()
{
  SHELL_VAR *v;

  v = init_seconds_var ();

  INIT_DYNAMIC_VAR ("BASH_COMMAND", (char *)NULL, get_bash_command, (sh_var_assign_func_t *)NULL);
  INIT_DYNAMIC_VAR ("BASH_SUBSHELL", (char *)NULL, get_subshell, assign_subshell);

  INIT_DYNAMIC_VAR ("RANDOM", (char *)NULL, get_random, assign_random);
  VSETATTR (v, att_integer);
  INIT_DYNAMIC_VAR ("LINENO", (char *)NULL, get_lineno, assign_lineno);
  VSETATTR (v, att_integer);

  INIT_DYNAMIC_VAR ("BASHPID", (char *)NULL, get_bashpid, null_assign);
  VSETATTR (v, att_integer|att_readonly);

#if defined (HISTORY)
  INIT_DYNAMIC_VAR ("HISTCMD", (char *)NULL, get_histcmd, (sh_var_assign_func_t *)NULL);
  VSETATTR (v, att_integer);
#endif

#if defined (READLINE)
  INIT_DYNAMIC_VAR ("COMP_WORDBREAKS", (char *)NULL, get_comp_wordbreaks, assign_comp_wordbreaks);
#endif

#if defined (PUSHD_AND_POPD) && defined (ARRAY_VARS)
  v = init_dynamic_array_var ("DIRSTACK", get_dirstack, assign_dirstack, 0);
#endif /* PUSHD_AND_POPD && ARRAY_VARS */

#if defined (ARRAY_VARS)
  v = init_dynamic_array_var ("GROUPS", get_groupset, null_array_assign, att_noassign);

#  if defined (DEBUGGER)
  v = init_dynamic_array_var ("BASH_ARGC", get_self, null_array_assign, att_noassign|att_nounset);
  v = init_dynamic_array_var ("BASH_ARGV", get_self, null_array_assign, att_noassign|att_nounset);
#  endif /* DEBUGGER */
  v = init_dynamic_array_var ("BASH_SOURCE", get_self, null_array_assign, att_noassign|att_nounset);
  v = init_dynamic_array_var ("BASH_LINENO", get_self, null_array_assign, att_noassign|att_nounset);

  v = init_dynamic_assoc_var ("BASH_CMDS", get_hashcmd, assign_hashcmd, att_nofree);
#  if defined (ALIAS)
  v = init_dynamic_assoc_var ("BASH_ALIASES", get_aliasvar, assign_aliasvar, att_nofree);
#  endif
#endif

  v = init_funcname_var ();
}