  void process_error(THD *thd)
  {
    if (context)
      context->process_error(thd);
  }