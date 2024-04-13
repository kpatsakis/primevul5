  virtual bool set_extraction_flag_processor(void *arg)
  {
    set_extraction_flag(*(int*)arg);
    return 0;
  }