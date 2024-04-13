static UnicodeString wrapUChars(const UChar *uchars) {
    return UnicodeString(TRUE, uchars, -1);
}