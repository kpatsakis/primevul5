UnicodeStringTest::TestSizeofUnicodeString() {
    // See the comments in unistr.h near the declaration of UnicodeString's fields.
    // See the API comments for UNISTR_OBJECT_SIZE.
    size_t sizeofUniStr=sizeof(UnicodeString);
    size_t expected=UNISTR_OBJECT_SIZE;
    if(expected!=sizeofUniStr) {
        // Possible cause: UNISTR_OBJECT_SIZE may not be a multiple of sizeof(pointer),
        // of the compiler might add more internal padding than expected.
        errln("sizeof(UnicodeString)=%d, expected UNISTR_OBJECT_SIZE=%d",
              (int)sizeofUniStr, (int)expected);
    }
    if(sizeofUniStr<32) {
        errln("sizeof(UnicodeString)=%d < 32, probably too small", (int)sizeofUniStr);
    }
    // We assume that the entire UnicodeString object,
    // minus the vtable pointer and 2 bytes for flags and short length,
    // is available for internal storage of UChars.
    int32_t expectedStackBufferLength=((int32_t)UNISTR_OBJECT_SIZE-sizeof(void *)-2)/U_SIZEOF_UCHAR;
    UnicodeString s;
    const UChar *emptyBuffer=s.getBuffer();
    for(int32_t i=0; i<expectedStackBufferLength; ++i) {
        s.append((UChar)0x2e);
    }
    const UChar *fullBuffer=s.getBuffer();
    if(fullBuffer!=emptyBuffer) {
        errln("unexpected reallocation when filling with assumed stack buffer size of %d",
              expectedStackBufferLength);
    }
    const UChar *terminatedBuffer=s.getTerminatedBuffer();
    if(terminatedBuffer==emptyBuffer) {
        errln("unexpected keeping stack buffer when overfilling assumed stack buffer size of %d",
              expectedStackBufferLength);
    }
}