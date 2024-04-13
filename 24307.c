uint maria_get_pointer_length(ulonglong file_length, uint def)
{
  DBUG_ASSERT(def >= 2 && def <= 7);
  if (file_length)				/* If not default */
  {
#ifdef NOT_YET_READY_FOR_8_BYTE_POINTERS
    if (file_length >= (1ULL << 56))
      def=8;
    else
#endif
      if (file_length >= (1ULL << 48))
      def=7;
    else if (file_length >= (1ULL << 40))
      def=6;
    else if (file_length >= (1ULL << 32))
      def=5;
    else if (file_length >= (1ULL << 24))
      def=4;
    else if (file_length >= (1ULL << 16))
      def=3;
    else
      def=2;
  }
  return def;
}