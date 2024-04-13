  enum Type real_type() const           { return ref ? (*ref)->type() :
                                          REF_ITEM; }