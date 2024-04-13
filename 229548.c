UnicodeStringTest::TestExtract()
{
    UnicodeString  test1("Now is the time for all good men to come to the aid of their country.", "");
    UnicodeString  test2;
    UChar          test3[13] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 10, 11, 12, 13};
    char           test4[13] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 10, 11, 12, 13};
    UnicodeString  test5;
    char           test6[13] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 10, 11, 12, 13};

    test1.extract(11, 12, test2);
    test1.extract(11, 12, test3);
    if (test1.extract(11, 12, test4) != 12 || test4[12] != 0) {
        errln("UnicodeString.extract(char *) failed to return the correct size of destination buffer.");
    }

    // test proper pinning in extractBetween()
    test1.extractBetween(-3, 7, test5);
    if(test5!=UNICODE_STRING("Now is ", 7)) {
        errln("UnicodeString.extractBetween(-3, 7) did not pin properly.");
    }

    test1.extractBetween(11, 23, test5);
    if (test1.extract(60, 71, test6) != 9) {
        errln("UnicodeString.extract() failed to return the correct size of destination buffer for end of buffer.");
    }
    if (test1.extract(11, 12, test6) != 12) {
        errln("UnicodeString.extract() failed to return the correct size of destination buffer.");
    }

    // convert test4 back to Unicode for comparison
    UnicodeString test4b(test4, 12);

    if (test1.extract(11, 12, (char *)NULL) != 12) {
        errln("UnicodeString.extract(NULL) failed to return the correct size of destination buffer.");
    }
    if (test1.extract(11, -1, test6) != 0) {
        errln("UnicodeString.extract(-1) failed to stop reading the string.");
    }

    for (int32_t i = 0; i < 12; i++) {
        if (test1.charAt((int32_t)(11 + i)) != test2.charAt(i)) {
            errln(UnicodeString("extracting into a UnicodeString failed at position ") + i);
            break;
        }
        if (test1.charAt((int32_t)(11 + i)) != test3[i]) {
            errln(UnicodeString("extracting into an array of UChar failed at position ") + i);
            break;
        }
        if (((char)test1.charAt((int32_t)(11 + i))) != test4b.charAt(i)) {
            errln(UnicodeString("extracting into an array of char failed at position ") + i);
            break;
        }
        if (test1.charAt((int32_t)(11 + i)) != test5.charAt(i)) {
            errln(UnicodeString("extracting with extractBetween failed at position ") + i);
            break;
        }
    }

    // test preflighting and overflows with invariant conversion
    if (test1.extract(0, 10, (char *)NULL, "") != 10) {
        errln("UnicodeString.extract(0, 10, (char *)NULL, \"\") != 10");
    }

    test4[2] = (char)0xff;
    if (test1.extract(0, 10, test4, 2, "") != 10) {
        errln("UnicodeString.extract(0, 10, test4, 2, \"\") != 10");
    }
    if (test4[2] != (char)0xff) {
        errln("UnicodeString.extract(0, 10, test4, 2, \"\") overwrote test4[2]");
    }

    {
        // test new, NUL-terminating extract() function
        UnicodeString s("terminate", "");
        UChar dest[20]={
            0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
            0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5
        };
        UErrorCode errorCode;
        int32_t length;

        errorCode=U_ZERO_ERROR;
        length=s.extract((UChar *)NULL, 0, errorCode);
        if(errorCode!=U_BUFFER_OVERFLOW_ERROR || length!=s.length()) {
            errln("UnicodeString.extract(NULL, 0)==%d (%s) expected %d (U_BUFFER_OVERFLOW_ERROR)", length, s.length(), u_errorName(errorCode));
        }

        errorCode=U_ZERO_ERROR;
        length=s.extract(dest, s.length()-1, errorCode);
        if(errorCode!=U_BUFFER_OVERFLOW_ERROR || length!=s.length()) {
            errln("UnicodeString.extract(dest too short)==%d (%s) expected %d (U_BUFFER_OVERFLOW_ERROR)",
                length, u_errorName(errorCode), s.length());
        }

        errorCode=U_ZERO_ERROR;
        length=s.extract(dest, s.length(), errorCode);
        if(errorCode!=U_STRING_NOT_TERMINATED_WARNING || length!=s.length()) {
            errln("UnicodeString.extract(dest just right without NUL)==%d (%s) expected %d (U_STRING_NOT_TERMINATED_WARNING)",
                length, u_errorName(errorCode), s.length());
        }
        if(dest[length-1]!=s[length-1] || dest[length]!=0xa5) {
            errln("UnicodeString.extract(dest just right without NUL) did not extract the string correctly");
        }

        errorCode=U_ZERO_ERROR;
        length=s.extract(dest, s.length()+1, errorCode);
        if(errorCode!=U_ZERO_ERROR || length!=s.length()) {
            errln("UnicodeString.extract(dest large enough)==%d (%s) expected %d (U_ZERO_ERROR)",
                length, u_errorName(errorCode), s.length());
        }
        if(dest[length-1]!=s[length-1] || dest[length]!=0 || dest[length+1]!=0xa5) {
            errln("UnicodeString.extract(dest large enough) did not extract the string correctly");
        }
    }

    {
        // test new UConverter extract() and constructor
        UnicodeString s=UNICODE_STRING("\\U0002f999\\U0001d15f\\u00c4\\u1ed0", 32).unescape();
        char buffer[32];
        static const char expect[]={
            (char)0xf0, (char)0xaf, (char)0xa6, (char)0x99,
            (char)0xf0, (char)0x9d, (char)0x85, (char)0x9f,
            (char)0xc3, (char)0x84,
            (char)0xe1, (char)0xbb, (char)0x90
        };
        UErrorCode errorCode=U_ZERO_ERROR;
        UConverter *cnv=ucnv_open("UTF-8", &errorCode);
        int32_t length;

        if(U_SUCCESS(errorCode)) {
            // test preflighting
            if( (length=s.extract(NULL, 0, cnv, errorCode))!=13 ||
                errorCode!=U_BUFFER_OVERFLOW_ERROR
            ) {
                errln("UnicodeString::extract(NULL, UConverter) preflighting failed (length=%ld, %s)",
                      length, u_errorName(errorCode));
            }
            errorCode=U_ZERO_ERROR;
            if( (length=s.extract(buffer, 2, cnv, errorCode))!=13 ||
                errorCode!=U_BUFFER_OVERFLOW_ERROR
            ) {
                errln("UnicodeString::extract(too small, UConverter) preflighting failed (length=%ld, %s)",
                      length, u_errorName(errorCode));
            }

            // try error cases
            errorCode=U_ZERO_ERROR;
            if( s.extract(NULL, 2, cnv, errorCode)==13 || U_SUCCESS(errorCode)) {
                errln("UnicodeString::extract(UConverter) succeeded with an illegal destination");
            }
            errorCode=U_ILLEGAL_ARGUMENT_ERROR;
            if( s.extract(NULL, 0, cnv, errorCode)==13 || U_SUCCESS(errorCode)) {
                errln("UnicodeString::extract(UConverter) succeeded with a previous error code");
            }
            errorCode=U_ZERO_ERROR;

            // extract for real
            if( (length=s.extract(buffer, sizeof(buffer), cnv, errorCode))!=13 ||
                uprv_memcmp(buffer, expect, 13)!=0 ||
                buffer[13]!=0 ||
                U_FAILURE(errorCode)
            ) {
                errln("UnicodeString::extract(UConverter) conversion failed (length=%ld, %s)",
                      length, u_errorName(errorCode));
            }
            // Test again with just the converter name.
            if( (length=s.extract(0, s.length(), buffer, sizeof(buffer), "UTF-8"))!=13 ||
                uprv_memcmp(buffer, expect, 13)!=0 ||
                buffer[13]!=0 ||
                U_FAILURE(errorCode)
            ) {
                errln("UnicodeString::extract(\"UTF-8\") conversion failed (length=%ld, %s)",
                      length, u_errorName(errorCode));
            }

            // try the constructor
            UnicodeString t(expect, sizeof(expect), cnv, errorCode);
            if(U_FAILURE(errorCode) || s!=t) {
                errln("UnicodeString(UConverter) conversion failed (%s)",
                      u_errorName(errorCode));
            }

            ucnv_close(cnv);
        }
    }
}