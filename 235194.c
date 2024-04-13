  bool copy_up_to(size_t bytes)
  {
    DBUG_ASSERT(bytes >= from);
    return dst->append(src + from, uint32(bytes - from));
  }