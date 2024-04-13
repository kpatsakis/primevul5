UnicodeString::toUTF8(ByteSink &sink) const {
  int32_t length16 = length();
  if(length16 != 0) {
    char stackBuffer[1024];
    int32_t capacity = (int32_t)sizeof(stackBuffer);
    UBool utf8IsOwned = FALSE;
    char *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      stackBuffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       NULL,    // Don't care about number of substitutions.
                       &errorCode);
    if(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = (char *)uprv_malloc(length8);
      if(utf8 != NULL) {
        utf8IsOwned = TRUE;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           NULL,    // Don't care about number of substitutions.
                           &errorCode);
      } else {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    if(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    if(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}