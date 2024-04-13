  enum precedence precedence() const
  {
    return ref ? (*ref)->precedence() : DEFAULT_PRECEDENCE;
  }