UnicodeStringTest::TestCountChar32(void) {
    {
        UnicodeString s=UNICODE_STRING("\\U0002f999\\U0001d15f\\u00c4\\u1ed0", 32).unescape();

        // test countChar32()
        // note that this also calls and tests u_countChar32(length>=0)
        if(
            s.countChar32()!=4 ||
            s.countChar32(1)!=4 ||
            s.countChar32(2)!=3 ||
            s.countChar32(2, 3)!=2 ||
            s.countChar32(2, 0)!=0
        ) {
            errln("UnicodeString::countChar32() failed");
        }

        // NUL-terminate the string buffer and test u_countChar32(length=-1)
        const UChar *buffer=s.getTerminatedBuffer();
        if(
            u_countChar32(buffer, -1)!=4 ||
            u_countChar32(buffer+1, -1)!=4 ||
            u_countChar32(buffer+2, -1)!=3 ||
            u_countChar32(buffer+3, -1)!=3 ||
            u_countChar32(buffer+4, -1)!=2 ||
            u_countChar32(buffer+5, -1)!=1 ||
            u_countChar32(buffer+6, -1)!=0
        ) {
            errln("u_countChar32(length=-1) failed");
        }

        // test u_countChar32() with bad input
        if(u_countChar32(NULL, 5)!=0 || u_countChar32(buffer, -2)!=0) {
            errln("u_countChar32(bad input) failed (returned non-zero counts)");
        }
    }

    /* test data and variables for hasMoreChar32Than() */
    static const UChar str[]={
        0x61, 0x62, 0xd800, 0xdc00,
        0xd801, 0xdc01, 0x63, 0xd802,
        0x64, 0xdc03, 0x65, 0x66,
        0xd804, 0xdc04, 0xd805, 0xdc05,
        0x67
    };
    UnicodeString string(str, UPRV_LENGTHOF(str));
    int32_t start, length, number;

    /* test hasMoreChar32Than() */
    for(length=string.length(); length>=0; --length) {
        for(start=0; start<=length; ++start) {
            for(number=-1; number<=((length-start)+2); ++number) {
                _testUnicodeStringHasMoreChar32Than(string, start, length-start, number);
            }
        }
    }

    /* test hasMoreChar32Than() with pinning */
    for(start=-1; start<=string.length()+1; ++start) {
        for(number=-1; number<=((string.length()-start)+2); ++number) {
            _testUnicodeStringHasMoreChar32Than(string, start, 0x7fffffff, number);
        }
    }

    /* test hasMoreChar32Than() with a bogus string */
    string.setToBogus();
    for(length=-1; length<=1; ++length) {
        for(start=-1; start<=length; ++start) {
            for(number=-1; number<=((length-start)+2); ++number) {
                _testUnicodeStringHasMoreChar32Than(string, start, length-start, number);
            }
        }
    }
}