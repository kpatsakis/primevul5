  virtual void update_used_tables()
  {
    if (field && field->default_value)
      field->default_value->expr->update_used_tables();
  }