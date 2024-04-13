UnicodeStringTest::TestBasicManipulation()
{
    UnicodeString   test1("Now is the time for all men to come swiftly to the aid of the party.\n");
    UnicodeString   expectedValue;
    UnicodeString   *c;

    c=test1.clone();
    test1.insert(24, "good ");
    expectedValue = "Now is the time for all good men to come swiftly to the aid of the party.\n";
    if (test1 != expectedValue)
        errln("insert() failed:  expected \"" + expectedValue + "\"\n,got \"" + test1 + "\"");

    c->insert(24, "good ");
    if(*c != expectedValue) {
        errln("clone()->insert() failed:  expected \"" + expectedValue + "\"\n,got \"" + *c + "\"");
    }
    delete c;

    test1.remove(41, 8);
    expectedValue = "Now is the time for all good men to come to the aid of the party.\n";
    if (test1 != expectedValue)
        errln("remove() failed:  expected \"" + expectedValue + "\"\n,got \"" + test1 + "\"");
    
    test1.replace(58, 6, "ir country");
    expectedValue = "Now is the time for all good men to come to the aid of their country.\n";
    if (test1 != expectedValue)
        errln("replace() failed:  expected \"" + expectedValue + "\"\n,got \"" + test1 + "\"");
    
    UChar     temp[80];
    test1.extract(0, 15, temp);
    
    UnicodeString       test2(temp, 15);
    
    expectedValue = "Now is the time";
    if (test2 != expectedValue)
        errln("extract() failed:  expected \"" + expectedValue + "\"\n,got \"" + test2 + "\"");
    
    test2 += " for me to go!\n";
    expectedValue = "Now is the time for me to go!\n";
    if (test2 != expectedValue)
        errln("operator+=() failed:  expected \"" + expectedValue + "\"\n,got \"" + test2 + "\"");
    
    if (test1.length() != 70)
        errln(UnicodeString("length() failed: expected 70, got ") + test1.length());
    if (test2.length() != 30)
        errln(UnicodeString("length() failed: expected 30, got ") + test2.length());

    UnicodeString test3;
    test3.append((UChar32)0x20402);
    if(test3 != CharsToUnicodeString("\\uD841\\uDC02")){
        errln((UnicodeString)"append failed for UChar32, expected \"\\\\ud841\\\\udc02\", got " + prettify(test3));
    }
    if(test3.length() != 2){
        errln(UnicodeString("append or length failed for UChar32, expected 2, got ") + test3.length());
    }
    test3.append((UChar32)0x0074);
    if(test3 != CharsToUnicodeString("\\uD841\\uDC02t")){
        errln((UnicodeString)"append failed for UChar32, expected \"\\\\uD841\\\\uDC02t\", got " + prettify(test3));
    }
    if(test3.length() != 3){
        errln((UnicodeString)"append or length failed for UChar32, expected 2, got " + test3.length());
    }

    // test some UChar32 overloads
    if( test3.setTo((UChar32)0x10330).length() != 2 ||
        test3.insert(0, (UChar32)0x20100).length() != 4 ||
        test3.replace(2, 2, (UChar32)0xe0061).length() != 4 ||
        (test3 = (UChar32)0x14001).length() != 2
    ) {
        errln((UnicodeString)"simple UChar32 overloads for replace, insert, setTo or = failed");
    }

    {
        // test moveIndex32()
        UnicodeString s=UNICODE_STRING("\\U0002f999\\U0001d15f\\u00c4\\u1ed0", 32).unescape();

        if(
            s.moveIndex32(2, -1)!=0 ||
            s.moveIndex32(2, 1)!=4 ||
            s.moveIndex32(2, 2)!=5 ||
            s.moveIndex32(5, -2)!=2 ||
            s.moveIndex32(0, -1)!=0 ||
            s.moveIndex32(6, 1)!=6
        ) {
            errln("UnicodeString::moveIndex32() failed");
        }

        if(s.getChar32Start(1)!=0 || s.getChar32Start(2)!=2) {
            errln("UnicodeString::getChar32Start() failed");
        }

        if(s.getChar32Limit(1)!=2 || s.getChar32Limit(2)!=2) {
            errln("UnicodeString::getChar32Limit() failed");
        }
    }

    {
        // test new 2.2 constructors and setTo function that parallel Java's substring function.
        UnicodeString src("Hello folks how are you?");
        UnicodeString target1("how are you?");
        if (target1 != UnicodeString(src, 12)) {
            errln("UnicodeString(const UnicodeString&, int32_t) failed");
        }
        UnicodeString target2("folks");
        if (target2 != UnicodeString(src, 6, 5)) {
            errln("UnicodeString(const UnicodeString&, int32_t, int32_t) failed");
        }
        if (target1 != target2.setTo(src, 12)) {
            errln("UnicodeString::setTo(const UnicodeString&, int32_t) failed");
        }
    }

    {
        // op+ is new in ICU 2.8
        UnicodeString s=UnicodeString("abc", "")+UnicodeString("def", "")+UnicodeString("ghi", "");
        if(s!=UnicodeString("abcdefghi", "")) {
            errln("operator+(UniStr, UniStr) failed");
        }
    }

    {
        // tests for Jitterbug 2360
        // verify that APIs with source pointer + length accept length == -1
        // mostly test only where modified, only few functions did not already do this
        if(UnicodeString("abc", -1, "")!=UnicodeString("abc", "")) {
            errln("UnicodeString(codepageData, dataLength, codepage) does not work with dataLength==-1");
        }

        UChar buffer[10]={ 0x61, 0x62, 0x20ac, 0xd900, 0xdc05, 0,   0x62, 0xffff, 0xdbff, 0xdfff };
        UnicodeString s, t(buffer, -1, UPRV_LENGTHOF(buffer));

        if(s.setTo(buffer, -1, UPRV_LENGTHOF(buffer)).length()!=u_strlen(buffer)) {
            errln("UnicodeString.setTo(buffer, length, capacity) does not work with length==-1");
        }
        if(t.length()!=u_strlen(buffer)) {
            errln("UnicodeString(buffer, length, capacity) does not work with length==-1");
        }

        if(0!=s.caseCompare(buffer, -1, U_FOLD_CASE_DEFAULT)) {
            errln("UnicodeString.caseCompare(const UChar *, length, options) does not work with length==-1");
        }
        if(0!=s.caseCompare(0, s.length(), buffer, U_FOLD_CASE_DEFAULT)) {
            errln("UnicodeString.caseCompare(start, _length, const UChar *, options) does not work");
        }

        buffer[u_strlen(buffer)]=0xe4;
        UnicodeString u(buffer, -1, UPRV_LENGTHOF(buffer));
        if(s.setTo(buffer, -1, UPRV_LENGTHOF(buffer)).length()!=UPRV_LENGTHOF(buffer)) {
            errln("UnicodeString.setTo(buffer without NUL, length, capacity) does not work with length==-1");
        }
        if(u.length()!=UPRV_LENGTHOF(buffer)) {
            errln("UnicodeString(buffer without NUL, length, capacity) does not work with length==-1");
        }

        static const char cs[]={ 0x61, (char)0xe4, (char)0x85, 0 };
        UConverter *cnv;
        UErrorCode errorCode=U_ZERO_ERROR;

        cnv=ucnv_open("ISO-8859-1", &errorCode);
        UnicodeString v(cs, -1, cnv, errorCode);
        ucnv_close(cnv);
        if(v!=CharsToUnicodeString("a\\xe4\\x85")) {
            errln("UnicodeString(const char *, length, cnv, errorCode) does not work with length==-1");
        }
    }

#if U_CHARSET_IS_UTF8
    {
        // Test the hardcoded-UTF-8 UnicodeString optimizations.
        static const uint8_t utf8[]={ 0x61, 0xC3, 0xA4, 0xC3, 0x9F, 0xE4, 0xB8, 0x80, 0 };
        static const UChar utf16[]={ 0x61, 0xE4, 0xDF, 0x4E00 };
        UnicodeString from8a = UnicodeString((const char *)utf8);
        UnicodeString from8b = UnicodeString((const char *)utf8, (int32_t)sizeof(utf8)-1);
        UnicodeString from16(FALSE, utf16, UPRV_LENGTHOF(utf16));
        if(from8a != from16 || from8b != from16) {
            errln("UnicodeString(const char * U_CHARSET_IS_UTF8) failed");
        }
        char buffer[16];
        int32_t length8=from16.extract(0, 0x7fffffff, buffer, (uint32_t)sizeof(buffer));
        if(length8!=((int32_t)sizeof(utf8)-1) || 0!=uprv_memcmp(buffer, utf8, sizeof(utf8))) {
            errln("UnicodeString::extract(char * U_CHARSET_IS_UTF8) failed");
        }
        length8=from16.extract(1, 2, buffer, (uint32_t)sizeof(buffer));
        if(length8!=4 || buffer[length8]!=0 || 0!=uprv_memcmp(buffer, utf8+1, length8)) {
            errln("UnicodeString::extract(substring to char * U_CHARSET_IS_UTF8) failed");
        }
    }
#endif
}