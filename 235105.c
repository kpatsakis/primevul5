  Field *make_num_distinct_aggregator_field(MEM_ROOT *mem_root,
                                            const Item *item) const
  {
    return type_handler()->make_num_distinct_aggregator_field(mem_root, this);
  }