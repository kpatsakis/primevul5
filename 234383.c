void thread_read_line()
{
   thread_rl_Running = 1;
   thread_rl_RetCode = ( read_line(PROMPT, 0) );
   thread_rl_Running = 0;
   DosPostEventSem(semInputReady);
}