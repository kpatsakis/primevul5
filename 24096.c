scm_dir_print (SCM exp, SCM port, scm_print_state *pstate SCM_UNUSED)
{
  scm_puts ("#<", port);
  if (!SCM_DIR_OPEN_P (exp))
    scm_puts ("closed: ", port);
  scm_puts ("directory stream ", port);
  scm_uintprint (SCM_SMOB_DATA_1 (exp), 16, port);
  scm_putc ('>', port);
  return 1;
}