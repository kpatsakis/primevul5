  bool check_vcol_func_processor(void *arg) 
  {
    if (example)
    {
      Item::vcol_func_processor_result *res= (Item::vcol_func_processor_result*)arg;
      example->check_vcol_func_processor(arg);
      /*
        Item_cache of a non-deterministic function requires re-fixing
        even if the function itself doesn't (e.g. CURRENT_TIMESTAMP)
      */
      if (res->errors & VCOL_NOT_STRICTLY_DETERMINISTIC)
        res->errors|= VCOL_SESSION_FUNC;
      return false;
    }
    return mark_unsupported_function("cache", arg, VCOL_IMPOSSIBLE);
  }