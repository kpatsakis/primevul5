  bool is_order_clause_position() const
  {
    return state == SHORT_DATA_VALUE &&
           type_handler()->is_order_clause_position_type();
  }