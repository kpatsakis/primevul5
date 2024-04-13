UnicodeString_charAt(int32_t offset, void *context) {
    return ((icu::UnicodeString*) context)->charAt(offset);
}