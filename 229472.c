UnicodeStringTest::TestCompare()
{
    UnicodeString   test1("this is a test");
    UnicodeString   test2("this is a test");
    UnicodeString   test3("this is a test of the emergency broadcast system");
    UnicodeString   test4("never say, \"this is a test\"!!");

    UnicodeString   test5((UChar)0x5000);
    UnicodeString   test6((UChar)0x5100);

    UChar         uniChars[] = { 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 
                 0x20, 0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0 };
    char            chars[] = "this is a test";

    // test operator== and operator!=
    if (test1 != test2 || test1 == test3 || test1 == test4)
        errln("operator== or operator!= failed");

    // test operator> and operator<
    if (test1 > test2 || test1 < test2 || !(test1 < test3) || !(test1 > test4) ||
        !(test5 < test6)
    ) {
        errln("operator> or operator< failed");
    }

    // test operator>= and operator<=
    if (!(test1 >= test2) || !(test1 <= test2) || !(test1 <= test3) || !(test1 >= test4))
        errln("operator>= or operator<= failed");

    // test compare(UnicodeString)
    if (test1.compare(test2) != 0 || test1.compare(test3) >= 0 || test1.compare(test4) <= 0)
        errln("compare(UnicodeString) failed");

    //test compare(offset, length, UnicodeString)
    if(test1.compare(0, 14, test2) != 0 ||
        test3.compare(0, 14, test2) != 0 ||
        test4.compare(12, 14, test2) != 0 ||
        test3.compare(0, 18, test1) <=0  )
        errln("compare(offset, length, UnicodeString) failes");

    // test compare(UChar*)
    if (test2.compare(uniChars) != 0 || test3.compare(uniChars) <= 0 || test4.compare(uniChars) >= 0)
        errln("compare(UChar*) failed");

    // test compare(char*)
    if (test2.compare(chars) != 0 || test3.compare(chars) <= 0 || test4.compare(chars) >= 0)
        errln("compare(char*) failed");

    // test compare(UChar*, length)
    if (test1.compare(uniChars, 4) <= 0 || test1.compare(uniChars, 4) <= 0)
        errln("compare(UChar*, length) failed");

    // test compare(thisOffset, thisLength, that, thatOffset, thatLength)
    if (test1.compare(0, 14, test2, 0, 14) != 0 
    || test1.compare(0, 14, test3, 0, 14) != 0
    || test1.compare(0, 14, test4, 12, 14) != 0)
        errln("1. compare(thisOffset, thisLength, that, thatOffset, thatLength) failed");

    if (test1.compare(10, 4, test2, 0, 4) >= 0 
    || test1.compare(10, 4, test3, 22, 9) <= 0
    || test1.compare(10, 4, test4, 22, 4) != 0)
        errln("2. compare(thisOffset, thisLength, that, thatOffset, thatLength) failed");

    // test compareBetween
    if (test1.compareBetween(0, 14, test2, 0, 14) != 0 || test1.compareBetween(0, 14, test3, 0, 14) != 0
                    || test1.compareBetween(0, 14, test4, 12, 26) != 0)
        errln("compareBetween failed");

    if (test1.compareBetween(10, 14, test2, 0, 4) >= 0 || test1.compareBetween(10, 14, test3, 22, 31) <= 0
                    || test1.compareBetween(10, 14, test4, 22, 26) != 0)
        errln("compareBetween failed");

    // test compare() etc. with strings that share a buffer but are not equal
    test2=test1; // share the buffer, length() too large for the stackBuffer
    test2.truncate(1); // change only the length, not the buffer
    if( test1==test2 || test1<=test2 ||
        test1.compare(test2)<=0 ||
        test1.compareCodePointOrder(test2)<=0 ||
        test1.compareCodePointOrder(0, INT32_MAX, test2)<=0 ||
        test1.compareCodePointOrder(0, INT32_MAX, test2, 0, INT32_MAX)<=0 ||
        test1.compareCodePointOrderBetween(0, INT32_MAX, test2, 0, INT32_MAX)<=0 ||
        test1.caseCompare(test2, U_FOLD_CASE_DEFAULT)<=0
    ) {
        errln("UnicodeStrings that share a buffer but have different lengths compare as equal");
    }

    /* test compareCodePointOrder() */
    {
        /* these strings are in ascending order */
        static const UChar strings[][4]={
            { 0x61, 0 },                    /* U+0061 */
            { 0x20ac, 0xd801, 0 },          /* U+20ac U+d801 */
            { 0x20ac, 0xd800, 0xdc00, 0 },  /* U+20ac U+10000 */
            { 0xd800, 0 },                  /* U+d800 */
            { 0xd800, 0xff61, 0 },          /* U+d800 U+ff61 */
            { 0xdfff, 0 },                  /* U+dfff */
            { 0xff61, 0xdfff, 0 },          /* U+ff61 U+dfff */
            { 0xff61, 0xd800, 0xdc02, 0 },  /* U+ff61 U+10002 */
            { 0xd800, 0xdc02, 0 },          /* U+10002 */
            { 0xd84d, 0xdc56, 0 }           /* U+23456 */
        };
        UnicodeString u[20]; // must be at least as long as strings[]
        int32_t i;

        for(i=0; i<UPRV_LENGTHOF(strings); ++i) {
            u[i]=UnicodeString(TRUE, strings[i], -1);
        }

        for(i=0; i<UPRV_LENGTHOF(strings)-1; ++i) {
            if(u[i].compareCodePointOrder(u[i+1])>=0 || u[i].compareCodePointOrder(0, INT32_MAX, u[i+1].getBuffer())>=0) {
                errln("error: UnicodeString::compareCodePointOrder() fails for string %d and the following one\n", i);
            }
        }
    }

    /* test caseCompare() */
    {
        static const UChar
        _mixed[]=               { 0x61, 0x42, 0x131, 0x3a3, 0xdf,       0x130,       0x49,  0xfb03,           0xd93f, 0xdfff, 0 },
        _otherDefault[]=        { 0x41, 0x62, 0x131, 0x3c3, 0x73, 0x53, 0x69, 0x307, 0x69,  0x46, 0x66, 0x49, 0xd93f, 0xdfff, 0 },
        _otherExcludeSpecialI[]={ 0x41, 0x62, 0x131, 0x3c3, 0x53, 0x73, 0x69,        0x131, 0x66, 0x46, 0x69, 0xd93f, 0xdfff, 0 },
        _different[]=           { 0x41, 0x62, 0x131, 0x3c3, 0x73, 0x53, 0x130,       0x49,  0x46, 0x66, 0x49, 0xd93f, 0xdffd, 0 };

        UnicodeString
            mixed(TRUE, _mixed, -1),
            otherDefault(TRUE, _otherDefault, -1),
            otherExcludeSpecialI(TRUE, _otherExcludeSpecialI, -1),
            different(TRUE, _different, -1);

        int8_t result;

        /* test caseCompare() */
        result=mixed.caseCompare(otherDefault, U_FOLD_CASE_DEFAULT);
        if(result!=0 || 0!=mixed.caseCompareBetween(0, INT32_MAX, otherDefault, 0, INT32_MAX, U_FOLD_CASE_DEFAULT)) {
            errln("error: mixed.caseCompare(other, default)=%ld instead of 0\n", result);
        }
        result=mixed.caseCompare(otherExcludeSpecialI, U_FOLD_CASE_EXCLUDE_SPECIAL_I);
        if(result!=0) {
            errln("error: mixed.caseCompare(otherExcludeSpecialI, U_FOLD_CASE_EXCLUDE_SPECIAL_I)=%ld instead of 0\n", result);
        }
        result=mixed.caseCompare(otherDefault, U_FOLD_CASE_EXCLUDE_SPECIAL_I);
        if(result==0 || 0==mixed.caseCompareBetween(0, INT32_MAX, otherDefault, 0, INT32_MAX, U_FOLD_CASE_EXCLUDE_SPECIAL_I)) {
            errln("error: mixed.caseCompare(other, U_FOLD_CASE_EXCLUDE_SPECIAL_I)=0 instead of !=0\n");
        }

        /* test caseCompare() */
        result=mixed.caseCompare(different, U_FOLD_CASE_DEFAULT);
        if(result<=0) {
            errln("error: mixed.caseCompare(different, default)=%ld instead of positive\n", result);
        }

        /* test caseCompare() - include the folded sharp s (U+00df) with different lengths */
        result=mixed.caseCompare(1, 4, different, 1, 5, U_FOLD_CASE_DEFAULT);
        if(result!=0 || 0!=mixed.caseCompareBetween(1, 5, different, 1, 6, U_FOLD_CASE_DEFAULT)) {
            errln("error: mixed.caseCompare(mixed, 1, 4, different, 1, 5, default)=%ld instead of 0\n", result);
        }

        /* test caseCompare() - stop in the middle of the sharp s (U+00df) */
        result=mixed.caseCompare(1, 4, different, 1, 4, U_FOLD_CASE_DEFAULT);
        if(result<=0) {
            errln("error: mixed.caseCompare(1, 4, different, 1, 4, default)=%ld instead of positive\n", result);
        }
    }

    // test that srcLength=-1 is handled in functions that
    // take input const UChar */int32_t srcLength (j785)
    {
        static const UChar u[]={ 0x61, 0x308, 0x62, 0 };
        UnicodeString s=UNICODE_STRING("a\\u0308b", 8).unescape();

        if(s.compare(u, -1)!=0 || s.compare(0, 999, u, 0, -1)!=0) {
            errln("error UnicodeString::compare(..., const UChar *, srcLength=-1) does not work");
        }

        if(s.compareCodePointOrder(u, -1)!=0 || s.compareCodePointOrder(0, 999, u, 0, -1)!=0) {
            errln("error UnicodeString::compareCodePointOrder(..., const UChar *, srcLength=-1, ...) does not work");
        }

        if(s.caseCompare(u, -1, U_FOLD_CASE_DEFAULT)!=0 || s.caseCompare(0, 999, u, 0, -1, U_FOLD_CASE_DEFAULT)!=0) {
            errln("error UnicodeString::caseCompare(..., const UChar *, srcLength=-1, ...) does not work");
        }

        if(s.indexOf(u, 1, -1, 0, 999)!=1 || s.indexOf(u+1, -1, 0, 999)!=1 || s.indexOf(u+1, -1, 0)!=1) {
            errln("error UnicodeString::indexOf(const UChar *, srcLength=-1, ...) does not work");
        }

        if(s.lastIndexOf(u, 1, -1, 0, 999)!=1 || s.lastIndexOf(u+1, -1, 0, 999)!=1 || s.lastIndexOf(u+1, -1, 0)!=1) {
            errln("error UnicodeString::lastIndexOf(const UChar *, srcLength=-1, ...) does not work");
        }

        UnicodeString s2, s3;
        s2.replace(0, 0, u+1, -1);
        s3.replace(0, 0, u, 1, -1);
        if(s.compare(1, 999, s2)!=0 || s2!=s3) {
            errln("error UnicodeString::replace(..., const UChar *, srcLength=-1, ...) does not work");
        }
    }
}