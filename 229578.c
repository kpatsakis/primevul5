UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) const {
  int32_t len = length();
  if(U_SUCCESS(errorCode)) {
    if(isBogus() || destCapacity<0 || (destCapacity>0 && dest==0)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } else {
      const UChar *array = getArrayStart();
      if(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      return u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  return len;
}