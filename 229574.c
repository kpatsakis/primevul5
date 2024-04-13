_refUnicodeStringHasMoreChar32Than(const UnicodeString &s, int32_t start, int32_t length, int32_t number) {
    int32_t count=s.countChar32(start, length);
    return count>number;
}