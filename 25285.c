in_string::~in_string()
{
  if (base)
  {
    // base was allocated on THD::mem_root => following is OK
    for (uint i=0 ; i < count ; i++)
      ((String*) base)[i].free();
  }
}