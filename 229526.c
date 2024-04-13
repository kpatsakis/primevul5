UnicodeStringTest::TestBogus() {
    UnicodeString   test1("This is a test");
    UnicodeString   test2("This is a test");
    UnicodeString   test3("Me too!");

    // test isBogus() and setToBogus()
    if (test1.isBogus() || test2.isBogus() || test3.isBogus()) {
        errln("A string returned TRUE for isBogus()!");
    }

    // NULL pointers are treated like empty strings
    // use other illegal arguments to make a bogus string
    test3.setTo(FALSE, test1.getBuffer(), -2);
    if(!test3.isBogus()) {
        errln("A bogus string returned FALSE for isBogus()!");
    }
    if (test1.hashCode() != test2.hashCode() || test1.hashCode() == test3.hashCode()) {
        errln("hashCode() failed");
    }
    if(test3.getBuffer()!=0 || test3.getBuffer(20)!=0 || test3.getTerminatedBuffer()!=0) {
        errln("bogus.getBuffer()!=0");
    }
    if (test1.indexOf(test3) != -1) {
        errln("bogus.indexOf() != -1");
    }
    if (test1.lastIndexOf(test3) != -1) {
        errln("bogus.lastIndexOf() != -1");
    }
    if (test1.caseCompare(test3, U_FOLD_CASE_DEFAULT) != 1 || test3.caseCompare(test1, U_FOLD_CASE_DEFAULT) != -1) {
        errln("caseCompare() doesn't work with bogus strings");
    }
    if (test1.compareCodePointOrder(test3) != 1 || test3.compareCodePointOrder(test1) != -1) {
        errln("compareCodePointOrder() doesn't work with bogus strings");
    }

    // verify that non-assignment modifications fail and do not revive a bogus string
    test3.setToBogus();
    test3.append((UChar)0x61);
    if(!test3.isBogus() || test3.getBuffer()!=0) {
        errln("bogus.append('a') worked but must not");
    }

    test3.setToBogus();
    test3.findAndReplace(UnicodeString((UChar)0x61), test2);
    if(!test3.isBogus() || test3.getBuffer()!=0) {
        errln("bogus.findAndReplace() worked but must not");
    }

    test3.setToBogus();
    test3.trim();
    if(!test3.isBogus() || test3.getBuffer()!=0) {
        errln("bogus.trim() revived bogus but must not");
    }

    test3.setToBogus();
    test3.remove(1);
    if(!test3.isBogus() || test3.getBuffer()!=0) {
        errln("bogus.remove(1) revived bogus but must not");
    }

    test3.setToBogus();
    if(!test3.setCharAt(0, 0x62).isBogus() || !test3.isEmpty()) {
        errln("bogus.setCharAt(0, 'b') worked but must not");
    }

    test3.setToBogus();
    if(test3.truncate(1) || !test3.isBogus() || !test3.isEmpty()) {
        errln("bogus.truncate(1) revived bogus but must not");
    }

    // verify that assignments revive a bogus string
    test3.setToBogus();
    if(!test3.isBogus() || (test3=test1).isBogus() || test3!=test1) {
        errln("bogus.operator=() failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.fastCopyFrom(test1).isBogus() || test3!=test1) {
        errln("bogus.fastCopyFrom() failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(test1).isBogus() || test3!=test1) {
        errln("bogus.setTo(UniStr) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(test1, 0).isBogus() || test3!=test1) {
        errln("bogus.setTo(UniStr, 0) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(test1, 0, 0x7fffffff).isBogus() || test3!=test1) {
        errln("bogus.setTo(UniStr, 0, len) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(test1.getBuffer(), test1.length()).isBogus() || test3!=test1) {
        errln("bogus.setTo(const UChar *, len) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo((UChar)0x2028).isBogus() || test3!=UnicodeString((UChar)0x2028)) {
        errln("bogus.setTo(UChar) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo((UChar32)0x1d157).isBogus() || test3!=UnicodeString((UChar32)0x1d157)) {
        errln("bogus.setTo(UChar32) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(FALSE, test1.getBuffer(), test1.length()).isBogus() || test3!=test1) {
        errln("bogus.setTo(readonly alias) failed");
    }

    // writable alias to another string's buffer: very bad idea, just convenient for this test
    test3.setToBogus();
    if(!test3.isBogus() ||
            test3.setTo(const_cast<UChar *>(test1.getBuffer()),
                        test1.length(), test1.getCapacity()).isBogus() ||
            test3!=test1) {
        errln("bogus.setTo(writable alias) failed");
    }

    // verify simple, documented ways to turn a bogus string into an empty one
    test3.setToBogus();
    if(!test3.isBogus() || (test3=UnicodeString()).isBogus() || !test3.isEmpty()) {
        errln("bogus.operator=(UnicodeString()) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(UnicodeString()).isBogus() || !test3.isEmpty()) {
        errln("bogus.setTo(UnicodeString()) failed");
    }

    test3.setToBogus();
    if(test3.remove().isBogus() || test3.getBuffer()==0 || !test3.isEmpty()) {
        errln("bogus.remove() failed");
    }

    test3.setToBogus();
    if(test3.remove(0, INT32_MAX).isBogus() || test3.getBuffer()==0 || !test3.isEmpty()) {
        errln("bogus.remove(0, INT32_MAX) failed");
    }

    test3.setToBogus();
    if(test3.truncate(0) || test3.isBogus() || !test3.isEmpty()) {
        errln("bogus.truncate(0) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo((UChar32)-1).isBogus() || !test3.isEmpty()) {
        errln("bogus.setTo((UChar32)-1) failed");
    }

    static const UChar nul=0;

    test3.setToBogus();
    if(!test3.isBogus() || test3.setTo(&nul, 0).isBogus() || !test3.isEmpty()) {
        errln("bogus.setTo(&nul, 0) failed");
    }

    test3.setToBogus();
    if(!test3.isBogus() || test3.getBuffer()!=0) {
        errln("setToBogus() failed to make a string bogus");
    }

    test3.setToBogus();
    if(test1.isBogus() || !(test1=test3).isBogus()) {
        errln("normal=bogus failed to make the left string bogus");
    }

    // test that NULL primitive input string values are treated like
    // empty strings, not errors (bogus)
    test2.setTo((UChar32)0x10005);
    if(test2.insert(1, nullptr, 1).length()!=2) {
        errln("UniStr.insert(...nullptr...) should not modify the string but does");
    }

    UErrorCode errorCode=U_ZERO_ERROR;
    UnicodeString
        test4((const UChar *)NULL),
        test5(TRUE, (const UChar *)NULL, 1),
        test6((UChar *)NULL, 5, 5),
        test7((const char *)NULL, 3, NULL, errorCode);
    if(test4.isBogus() || test5.isBogus() || test6.isBogus() || test7.isBogus()) {
        errln("a constructor set to bogus for a NULL input string, should be empty");
    }

    test4.setTo(NULL, 3);
    test5.setTo(TRUE, (const UChar *)NULL, 1);
    test6.setTo((UChar *)NULL, 5, 5);
    if(test4.isBogus() || test5.isBogus() || test6.isBogus()) {
        errln("a setTo() set to bogus for a NULL input string, should be empty");
    }

    // test that bogus==bogus<any
    if(test1!=test3 || test1.compare(test3)!=0) {
        errln("bogus==bogus failed");
    }

    test2.remove();
    if(test1>=test2 || !(test2>test1) || test1.compare(test2)>=0 || !(test2.compare(test1)>0)) {
        errln("bogus<empty failed");
    }
}