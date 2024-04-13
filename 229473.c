UnicodeStringTest::_testUnicodeStringHasMoreChar32Than(const UnicodeString &s, int32_t start, int32_t length, int32_t number) {
    if(s.hasMoreChar32Than(start, length, number)!=_refUnicodeStringHasMoreChar32Than(s, start, length, number)) {
        errln("hasMoreChar32Than(%d, %d, %d)=%hd is wrong\n",
                start, length, number, s.hasMoreChar32Than(start, length, number));
    }
}