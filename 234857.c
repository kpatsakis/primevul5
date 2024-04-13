  void set_extraction_flag(int flags) 
  { 
    marker &= ~EXTRACTION_MASK;
    marker|= flags; 
  }