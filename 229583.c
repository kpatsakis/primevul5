static void uprv_UnicodeStringDummy(void) {
    delete [] (new UnicodeString[2]);
}