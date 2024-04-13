static void noopStepFunc(    /*NO_TEST*/
  sqlite3_context *p,        /*NO_TEST*/
  int n,                     /*NO_TEST*/
  sqlite3_value **a          /*NO_TEST*/
){                           /*NO_TEST*/
  UNUSED_PARAMETER(p);       /*NO_TEST*/
  UNUSED_PARAMETER(n);       /*NO_TEST*/
  UNUSED_PARAMETER(a);       /*NO_TEST*/
  assert(0);                 /*NO_TEST*/
}                            /*NO_TEST*/