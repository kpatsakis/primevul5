tape_offline (int tape_des)
{
#if defined(MTIOCTOP) && defined(MTOFFL)
  struct mtop control;

  control.mt_op = MTOFFL;
  control.mt_count = 1;
  rmtioctl (tape_des, MTIOCTOP, (char*) &control);	/* Don't care if it fails.  */
#endif
}