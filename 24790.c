  virtual ~Item()
  {
#ifdef EXTRA_DEBUG
    name.str= 0;
    name.length= 0;
#endif
  }		/*lint -e1509 */