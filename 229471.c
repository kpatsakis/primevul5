UnicodeStringTest::TestMoveSwap() {
    static const UChar abc[3] = { 0x61, 0x62, 0x63 };  // "abc"
    UnicodeString s1(FALSE, abc, UPRV_LENGTHOF(abc));  // read-only alias
    UnicodeString s2(100, 0x7a, 100);  // 100 * 'z' should be on the heap
    UnicodeString s3("defg", 4, US_INV);  // in stack buffer
    const UChar *p = s2.getBuffer();
    s1.swap(s2);
    if(s1.getBuffer() != p || s1.length() != 100 || s2.getBuffer() != abc || s2.length() != 3) {
        errln("UnicodeString.swap() did not swap");
    }
    swap(s2, s3);
    if(s2 != UNICODE_STRING_SIMPLE("defg") || s3.getBuffer() != abc || s3.length() != 3) {
        errln("swap(UnicodeString) did not swap back");
    }
    UnicodeString s4;
    s4 = std::move(s1);
    if(s4.getBuffer() != p || s4.length() != 100 || !s1.isBogus()) {
        errln("UnicodeString = std::move(heap) did not move");
    }
    UnicodeString s5;
    s5 = std::move(s2);
    if(s5 != UNICODE_STRING_SIMPLE("defg")) {
        errln("UnicodeString = std::move(stack) did not move");
    }
    UnicodeString s6;
    s6 = std::move(s3);
    if(s6.getBuffer() != abc || s6.length() != 3) {
        errln("UnicodeString = std::move(alias) did not move");
    }
    infoln("TestMoveSwap() with rvalue references");
    s1 = static_cast<UnicodeString &&>(s6);
    if(s1.getBuffer() != abc || s1.length() != 3) {
        errln("UnicodeString move assignment operator did not move");
    }
    UnicodeString s7(static_cast<UnicodeString &&>(s4));
    if(s7.getBuffer() != p || s7.length() != 100 || !s4.isBogus()) {
        errln("UnicodeString move constructor did not move");
    }

    // Move self assignment leaves the object valid but in an undefined state.
    // Do it to make sure there is no crash,
    // but do not check for any particular resulting value.
    moveFrom(s1, s1);
    moveFrom(s2, s2);
    moveFrom(s3, s3);
    moveFrom(s4, s4);
    moveFrom(s5, s5);
    moveFrom(s6, s6);
    moveFrom(s7, s7);
    // Simple copy assignment must work.
    UnicodeString simple = UNICODE_STRING_SIMPLE("simple");
    s1 = s6 = s4 = s7 = simple;
    if(s1 != simple || s4 != simple || s6 != simple || s7 != simple) {
        errln("UnicodeString copy after self-move did not work");
    }
}