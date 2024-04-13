time_command (command, asynchronous, pipe_in, pipe_out, fds_to_close)
     COMMAND *command;
     int asynchronous, pipe_in, pipe_out;
     struct fd_bitmap *fds_to_close;
{
  int rv, posix_time, old_flags, nullcmd;
  time_t rs, us, ss;
  int rsf, usf, ssf;
  int cpu;
  char *time_format;

#if defined (HAVE_GETRUSAGE) && defined (HAVE_GETTIMEOFDAY)
  struct timeval real, user, sys;
  struct timeval before, after;
#  if defined (HAVE_STRUCT_TIMEZONE)
  struct timezone dtz;				/* posix doesn't define this */
#  endif
  struct rusage selfb, selfa, kidsb, kidsa;	/* a = after, b = before */
#else
#  if defined (HAVE_TIMES)
  clock_t tbefore, tafter, real, user, sys;
  struct tms before, after;
#  endif
#endif

#if defined (HAVE_GETRUSAGE) && defined (HAVE_GETTIMEOFDAY)
#  if defined (HAVE_STRUCT_TIMEZONE)
  gettimeofday (&before, &dtz);
#  else
  gettimeofday (&before, (void *)NULL);
#  endif /* !HAVE_STRUCT_TIMEZONE */
  getrusage (RUSAGE_SELF, &selfb);
  getrusage (RUSAGE_CHILDREN, &kidsb);
#else
#  if defined (HAVE_TIMES)
  tbefore = times (&before);
#  endif
#endif

  posix_time = command && (command->flags & CMD_TIME_POSIX);

  nullcmd = (command == 0) || (command->type == cm_simple && command->value.Simple->words == 0 && command->value.Simple->redirects == 0);
  if (posixly_correct && nullcmd)
    {
#if defined (HAVE_GETRUSAGE)
      selfb.ru_utime.tv_sec = kidsb.ru_utime.tv_sec = selfb.ru_stime.tv_sec = kidsb.ru_stime.tv_sec = 0;
      selfb.ru_utime.tv_usec = kidsb.ru_utime.tv_usec = selfb.ru_stime.tv_usec = kidsb.ru_stime.tv_usec = 0;
      before.tv_sec = shell_start_time;
      before.tv_usec = 0;
#else
      before.tms_utime = before.tms_stime = before.tms_cutime = before.tms_cstime = 0;
      tbefore = shell_start_time;
#endif
    }

  old_flags = command->flags;
  command->flags &= ~(CMD_TIME_PIPELINE|CMD_TIME_POSIX);
  rv = execute_command_internal (command, asynchronous, pipe_in, pipe_out, fds_to_close);
  command->flags = old_flags;

  rs = us = ss = 0;
  rsf = usf = ssf = cpu = 0;

#if defined (HAVE_GETRUSAGE) && defined (HAVE_GETTIMEOFDAY)
#  if defined (HAVE_STRUCT_TIMEZONE)
  gettimeofday (&after, &dtz);
#  else
  gettimeofday (&after, (void *)NULL);
#  endif /* !HAVE_STRUCT_TIMEZONE */
  getrusage (RUSAGE_SELF, &selfa);
  getrusage (RUSAGE_CHILDREN, &kidsa);

  difftimeval (&real, &before, &after);
  timeval_to_secs (&real, &rs, &rsf);

  addtimeval (&user, difftimeval(&after, &selfb.ru_utime, &selfa.ru_utime),
		     difftimeval(&before, &kidsb.ru_utime, &kidsa.ru_utime));
  timeval_to_secs (&user, &us, &usf);

  addtimeval (&sys, difftimeval(&after, &selfb.ru_stime, &selfa.ru_stime),
		    difftimeval(&before, &kidsb.ru_stime, &kidsa.ru_stime));
  timeval_to_secs (&sys, &ss, &ssf);

  cpu = timeval_to_cpu (&real, &user, &sys);
#else
#  if defined (HAVE_TIMES)
  tafter = times (&after);

  real = tafter - tbefore;
  clock_t_to_secs (real, &rs, &rsf);

  user = (after.tms_utime - before.tms_utime) + (after.tms_cutime - before.tms_cutime);
  clock_t_to_secs (user, &us, &usf);

  sys = (after.tms_stime - before.tms_stime) + (after.tms_cstime - before.tms_cstime);
  clock_t_to_secs (sys, &ss, &ssf);

  cpu = (real == 0) ? 0 : ((user + sys) * 10000) / real;

#  else
  rs = us = ss = 0;
  rsf = usf = ssf = cpu = 0;
#  endif
#endif

  if (posix_time)
    time_format = POSIX_TIMEFORMAT;
  else if ((time_format = get_string_value ("TIMEFORMAT")) == 0)
    {
      if (posixly_correct && nullcmd)
	time_format = "user\t%2lU\nsys\t%2lS";
      else
	time_format = BASH_TIMEFORMAT;
    }
  if (time_format && *time_format)
    print_formatted_time (stderr, time_format, rs, rsf, us, usf, ss, ssf, cpu);

  return rv;
}