UnicodeStringTest::TestReadOnlyAlias() {
    UChar uchars[]={ 0x61, 0x62, 0 };
    UnicodeString alias(TRUE, uchars, 2);
    if(alias.length()!=2 || alias.getBuffer()!=uchars || alias.getTerminatedBuffer()!=uchars) {
        errln("UnicodeString read-only-aliasing constructor does not behave as expected.");
        return;
    }
    alias.truncate(1);
    if(alias.length()!=1 || alias.getBuffer()!=uchars) {
        errln("UnicodeString(read-only-alias).truncate() did not preserve aliasing as expected.");
    }
    if(alias.getTerminatedBuffer()==uchars) {
        errln("UnicodeString(read-only-alias).truncate().getTerminatedBuffer() "
              "did not allocate and copy as expected.");
    }
    if(uchars[1]!=0x62) {
        errln("UnicodeString(read-only-alias).truncate().getTerminatedBuffer() "
              "modified the original buffer.");
    }
    if(1!=u_strlen(alias.getTerminatedBuffer())) {
        errln("UnicodeString(read-only-alias).truncate().getTerminatedBuffer() "
              "does not return a buffer terminated at the proper length.");
    }

    alias.setTo(TRUE, uchars, 2);
    if(alias.length()!=2 || alias.getBuffer()!=uchars || alias.getTerminatedBuffer()!=uchars) {
        errln("UnicodeString read-only-aliasing setTo() does not behave as expected.");
        return;
    }
    alias.remove();
    if(alias.length()!=0) {
        errln("UnicodeString(read-only-alias).remove() did not work.");
    }
    if(alias.getTerminatedBuffer()==uchars) {
        errln("UnicodeString(read-only-alias).remove().getTerminatedBuffer() "
              "did not un-alias as expected.");
    }
    if(uchars[0]!=0x61) {
        errln("UnicodeString(read-only-alias).remove().getTerminatedBuffer() "
              "modified the original buffer.");
    }
    if(0!=u_strlen(alias.getTerminatedBuffer())) {
        errln("UnicodeString.setTo(read-only-alias).remove().getTerminatedBuffer() "
              "does not return a buffer terminated at length 0.");
    }

    UnicodeString longString=UNICODE_STRING_SIMPLE("abcdefghijklmnopqrstuvwxyz0123456789");
    alias.setTo(FALSE, longString.getBuffer(), longString.length());
    alias.remove(0, 10);
    if(longString.compare(10, INT32_MAX, alias)!=0 || alias.getBuffer()!=longString.getBuffer()+10) {
        errln("UnicodeString.setTo(read-only-alias).remove(0, 10) did not preserve aliasing as expected.");
    }
    alias.setTo(FALSE, longString.getBuffer(), longString.length());
    alias.remove(27, 99);
    if(longString.compare(0, 27, alias)!=0 || alias.getBuffer()!=longString.getBuffer()) {
        errln("UnicodeString.setTo(read-only-alias).remove(27, 99) did not preserve aliasing as expected.");
    }
    alias.setTo(FALSE, longString.getBuffer(), longString.length());
    alias.retainBetween(6, 30);
    if(longString.compare(6, 24, alias)!=0 || alias.getBuffer()!=longString.getBuffer()+6) {
        errln("UnicodeString.setTo(read-only-alias).retainBetween(6, 30) did not preserve aliasing as expected.");
    }

    UChar abc[]={ 0x61, 0x62, 0x63, 0 };
    UBool hasRVO= wrapUChars(abc).getBuffer()==abc;

    UnicodeString temp;
    temp.fastCopyFrom(longString.tempSubString());
    if(temp!=longString || (hasRVO && temp.getBuffer()!=longString.getBuffer())) {
        errln("UnicodeString.tempSubString() failed");
    }
    temp.fastCopyFrom(longString.tempSubString(-3, 5));
    if(longString.compare(0, 5, temp)!=0 || (hasRVO && temp.getBuffer()!=longString.getBuffer())) {
        errln("UnicodeString.tempSubString(-3, 5) failed");
    }
    temp.fastCopyFrom(longString.tempSubString(17));
    if(longString.compare(17, INT32_MAX, temp)!=0 || (hasRVO && temp.getBuffer()!=longString.getBuffer()+17)) {
        errln("UnicodeString.tempSubString(17) failed");
    }
    temp.fastCopyFrom(longString.tempSubString(99));
    if(!temp.isEmpty()) {
        errln("UnicodeString.tempSubString(99) failed");
    }
    temp.fastCopyFrom(longString.tempSubStringBetween(6));
    if(longString.compare(6, INT32_MAX, temp)!=0 || (hasRVO && temp.getBuffer()!=longString.getBuffer()+6)) {
        errln("UnicodeString.tempSubStringBetween(6) failed");
    }
    temp.fastCopyFrom(longString.tempSubStringBetween(8, 18));
    if(longString.compare(8, 10, temp)!=0 || (hasRVO && temp.getBuffer()!=longString.getBuffer()+8)) {
        errln("UnicodeString.tempSubStringBetween(8, 18) failed");
    }
    UnicodeString bogusString;
    bogusString.setToBogus();
    temp.fastCopyFrom(bogusString.tempSubStringBetween(8, 18));
    if(!temp.isBogus()) {
        errln("UnicodeString.setToBogus().tempSubStringBetween(8, 18) failed");
    }
}