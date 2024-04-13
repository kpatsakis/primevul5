sort_variables (array)
     SHELL_VAR **array;
{
  qsort (array, strvec_len ((char **)array), sizeof (SHELL_VAR *), (QSFUNC *)qsort_var_comp);
}