void Regexp_processor_pcre::set_recursion_limit(THD *thd)
{
  long stack_used;
  DBUG_ASSERT(thd == current_thd);
  stack_used= available_stack_size(thd->thread_stack, &stack_used);
  m_pcre_extra.match_limit_recursion=
    (ulong)((my_thread_stack_size - STACK_MIN_SIZE - stack_used)/my_pcre_frame_size);
}