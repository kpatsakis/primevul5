UnicodeStringTest::doTestAppendable(UnicodeString &dest, Appendable &app) {
    static const UChar cde[3]={ 0x63, 0x64, 0x65 };
    static const UChar fg[3]={ 0x66, 0x67, 0 };
    if(!app.reserveAppendCapacity(12)) {
        errln("Appendable.reserve(12) failed");
    }
    app.appendCodeUnit(0x61);
    app.appendCodePoint(0x62);
    app.appendCodePoint(0x50000);
    app.appendString(cde, 3);
    app.appendString(fg, -1);
    UChar scratch[3];
    int32_t capacity=-1;
    UChar *buffer=app.getAppendBuffer(3, 3, scratch, 3, &capacity);
    if(capacity<3) {
        errln("Appendable.getAppendBuffer(min=3) returned capacity=%d<3", (int)capacity);
        return;
    }
    static const UChar hij[3]={ 0x68, 0x69, 0x6a };
    u_memcpy(buffer, hij, 3);
    app.appendString(buffer, 3);
    if(dest!=UNICODE_STRING_SIMPLE("ab\\U00050000cdefghij").unescape()) {
        errln("Appendable.append(...) failed");
    }
    buffer=app.getAppendBuffer(0, 3, scratch, 3, &capacity);
    if(buffer!=NULL || capacity!=0) {
        errln("Appendable.getAppendBuffer(min=0) failed");
    }
    capacity=1;
    buffer=app.getAppendBuffer(3, 3, scratch, 2, &capacity);
    if(buffer!=NULL || capacity!=0) {
        errln("Appendable.getAppendBuffer(scratch<min) failed");
    }
}