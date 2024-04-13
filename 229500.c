UnicodeStringTest::TestStringEnumeration() {
    UnicodeString s;
    TestEnumeration ten;
    int32_t i, length;
    UErrorCode status;

    const UChar *pu;
    const char *pc;

    // test the next() default implementation and ensureCharsCapacity()
    for(i=0; i<UPRV_LENGTHOF(testEnumStrings); ++i) {
        status=U_ZERO_ERROR;
        pc=ten.next(&length, status);
        s=UnicodeString(testEnumStrings[i], "");
        if(U_FAILURE(status) || pc==NULL || length!=s.length() || UnicodeString(pc, length, "")!=s) {
            errln("StringEnumeration.next(%d) failed", i);
        }
    }
    status=U_ZERO_ERROR;
    if(ten.next(&length, status)!=NULL) {
        errln("StringEnumeration.next(done)!=NULL");
    }

    // test the unext() default implementation
    ten.reset(status);
    for(i=0; i<UPRV_LENGTHOF(testEnumStrings); ++i) {
        status=U_ZERO_ERROR;
        pu=ten.unext(&length, status);
        s=UnicodeString(testEnumStrings[i], "");
        if(U_FAILURE(status) || pu==NULL || length!=s.length() || UnicodeString(TRUE, pu, length)!=s) {
            errln("StringEnumeration.unext(%d) failed", i);
        }
    }
    status=U_ZERO_ERROR;
    if(ten.unext(&length, status)!=NULL) {
        errln("StringEnumeration.unext(done)!=NULL");
    }

    // test that the default clone() implementation works, and returns NULL
    if(ten.clone()!=NULL) {
        errln("StringEnumeration.clone()!=NULL");
    }

    // test that uenum_openFromStringEnumeration() works
    // Need a heap allocated string enumeration because it is adopted by the UEnumeration.
    StringEnumeration *newTen = new TestEnumeration;
    status=U_ZERO_ERROR;
    UEnumeration *uten = uenum_openFromStringEnumeration(newTen, &status);
    if (uten==NULL || U_FAILURE(status)) {
        errln("fail at file %s, line %d, UErrorCode is %s\n", __FILE__, __LINE__, u_errorName(status));
        return;
    }
    
    // test  uenum_next()
    for(i=0; i<UPRV_LENGTHOF(testEnumStrings); ++i) {
        status=U_ZERO_ERROR;
        pc=uenum_next(uten, &length, &status);
        if(U_FAILURE(status) || pc==NULL || strcmp(pc, testEnumStrings[i]) != 0) {
            errln("File %s, line %d, StringEnumeration.next(%d) failed", __FILE__, __LINE__, i);
        }
    }
    status=U_ZERO_ERROR;
    if(uenum_next(uten, &length, &status)!=NULL) {
        errln("File %s, line %d, uenum_next(done)!=NULL");
    }

    // test the uenum_unext()
    uenum_reset(uten, &status);
    for(i=0; i<UPRV_LENGTHOF(testEnumStrings); ++i) {
        status=U_ZERO_ERROR;
        pu=uenum_unext(uten, &length, &status);
        s=UnicodeString(testEnumStrings[i], "");
        if(U_FAILURE(status) || pu==NULL || length!=s.length() || UnicodeString(TRUE, pu, length)!=s) {
            errln("File %s, Line %d, uenum_unext(%d) failed", __FILE__, __LINE__, i);
        }
    }
    status=U_ZERO_ERROR;
    if(uenum_unext(uten, &length, &status)!=NULL) {
        errln("File %s, Line %d, uenum_unext(done)!=NULL" __FILE__, __LINE__);
    }

    uenum_close(uten);
}