  TYPELIB *get_typelib() const
  {
    return ref ? (*ref)->get_typelib() : NULL;
  }