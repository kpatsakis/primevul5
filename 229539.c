UnicodeStringTest::TestMiscellaneous()
{
    UnicodeString   test1("This is a test");
    UnicodeString   test2("This is a test");
    UnicodeString   test3("Me too!");

    // test getBuffer(minCapacity) and releaseBuffer()
    test1=UnicodeString(); // make sure that it starts with its stackBuffer
    UChar *p=test1.getBuffer(20);
    if(test1.getCapacity()<20) {
        errln("UnicodeString::getBuffer(20).getCapacity()<20");
    }

    test1.append((UChar)7); // must not be able to modify the string here
    test1.setCharAt(3, 7);
    test1.reverse();
    if( test1.length()!=0 ||
        test1.charAt(0)!=0xffff || test1.charAt(3)!=0xffff ||
        test1.getBuffer(10)!=0 || test1.getBuffer()!=0
    ) {
        errln("UnicodeString::getBuffer(minCapacity) allows read or write access to the UnicodeString");
    }

    p[0]=1;
    p[1]=2;
    p[2]=3;
    test1.releaseBuffer(3);
    test1.append((UChar)4);

    if(test1.length()!=4 || test1.charAt(0)!=1 || test1.charAt(1)!=2 || test1.charAt(2)!=3 || test1.charAt(3)!=4) {
        errln("UnicodeString::releaseBuffer(newLength) does not properly reallow access to the UnicodeString");
    }

    // test releaseBuffer() without getBuffer(minCapacity) - must not have any effect
    test1.releaseBuffer(1);
    if(test1.length()!=4 || test1.charAt(0)!=1 || test1.charAt(1)!=2 || test1.charAt(2)!=3 || test1.charAt(3)!=4) {
        errln("UnicodeString::releaseBuffer(newLength) without getBuffer(minCapacity) changed the UnicodeString");
    }

    // test getBuffer(const)
    const UChar *q=test1.getBuffer(), *r=test1.getBuffer();
    if( test1.length()!=4 ||
        q[0]!=1 || q[1]!=2 || q[2]!=3 || q[3]!=4 ||
        r[0]!=1 || r[1]!=2 || r[2]!=3 || r[3]!=4
    ) {
        errln("UnicodeString::getBuffer(const) does not return a usable buffer pointer");
    }

    // test releaseBuffer() with a NUL-terminated buffer
    test1.getBuffer(20)[2]=0;
    test1.releaseBuffer(); // implicit -1
    if(test1.length()!=2 || test1.charAt(0)!=1 || test1.charAt(1) !=2) {
        errln("UnicodeString::releaseBuffer(-1) does not properly set the length of the UnicodeString");
    }

    // test releaseBuffer() with a non-NUL-terminated buffer
    p=test1.getBuffer(256);
    for(int32_t i=0; i<test1.getCapacity(); ++i) {
        p[i]=(UChar)1;      // fill the buffer with all non-NUL code units
    }
    test1.releaseBuffer();  // implicit -1
    if(test1.length()!=test1.getCapacity() || test1.charAt(1)!=1 || test1.charAt(100)!=1 || test1.charAt(test1.getCapacity()-1)!=1) {
        errln("UnicodeString::releaseBuffer(-1 but no NUL) does not properly set the length of the UnicodeString");
    }

    // test getTerminatedBuffer()
    test1=UnicodeString("This is another test.", "");
    test2=UnicodeString("This is another test.", "");
    q=test1.getTerminatedBuffer();
    if(q[test1.length()]!=0 || test1!=test2 || test2.compare(q, -1)!=0) {
        errln("getTerminatedBuffer()[length]!=0");
    }

    const UChar u[]={ 5, 6, 7, 8, 0 };
    test1.setTo(FALSE, u, 3);
    q=test1.getTerminatedBuffer();
    if(q==u || q[0]!=5 || q[1]!=6 || q[2]!=7 || q[3]!=0) {
        errln("UnicodeString(u[3]).getTerminatedBuffer() returns a bad buffer");
    }

    test1.setTo(TRUE, u, -1);
    q=test1.getTerminatedBuffer();
    if(q!=u || test1.length()!=4 || q[3]!=8 || q[4]!=0) {
        errln("UnicodeString(u[-1]).getTerminatedBuffer() returns a bad buffer");
    }

    // NOTE: Some compilers will optimize u"la" to point to the same static memory
    // as u" lila", offset by 3 code units
    test1=UnicodeString(TRUE, u"la", 2);
    test1.append(UnicodeString(TRUE, u" lila", 5).getTerminatedBuffer(), 0, -1);
    assertEquals("UnicodeString::append(const UChar *, start, length) failed",
        u"la lila", test1);

    test1.insert(3, UnicodeString(TRUE, u"dudum ", 6), 0, INT32_MAX);
    assertEquals("UnicodeString::insert(start, const UniStr &, start, length) failed",
        u"la dudum lila", test1);

    static const UChar ucs[]={ 0x68, 0x6d, 0x20, 0 };
    test1.insert(9, ucs, -1);
    assertEquals("UnicodeString::insert(start, const UChar *, length) failed",
        u"la dudum hm lila", test1);

    test1.replace(9, 2, (UChar)0x2b);
    assertEquals("UnicodeString::replace(start, length, UChar) failed",
        u"la dudum + lila", test1);

    if(test1.hasMetaData() || UnicodeString().hasMetaData()) {
        errln("UnicodeString::hasMetaData() returns TRUE");
    }

    // test getTerminatedBuffer() on a truncated, shared, heap-allocated string
    test1=UNICODE_STRING_SIMPLE("abcdefghijklmnopqrstuvwxyz0123456789.");
    test1.truncate(36);  // ensure length()<getCapacity()
    test2=test1;  // share the buffer
    test1.truncate(5);
    if(test1.length()!=5 || test1.getTerminatedBuffer()[5]!=0) {
        errln("UnicodeString(shared buffer).truncate() failed");
    }
    if(test2.length()!=36 || test2[5]!=0x66 || u_strlen(test2.getTerminatedBuffer())!=36) {
        errln("UnicodeString(shared buffer).truncate().getTerminatedBuffer() "
              "modified another copy of the string!");
    }
    test1=UNICODE_STRING_SIMPLE("abcdefghijklmnopqrstuvwxyz0123456789.");
    test1.truncate(36);  // ensure length()<getCapacity()
    test2=test1;  // share the buffer
    test1.remove();
    if(test1.length()!=0 || test1.getTerminatedBuffer()[0]!=0) {
        errln("UnicodeString(shared buffer).remove() failed");
    }
    if(test2.length()!=36 || test2[0]!=0x61 || u_strlen(test2.getTerminatedBuffer())!=36) {
        errln("UnicodeString(shared buffer).remove().getTerminatedBuffer() "
              "modified another copy of the string!");
    }

    // ticket #9740
    test1.setTo(TRUE, ucs, 3);
    assertEquals("length of read-only alias", 3, test1.length());
    test1.trim();
    assertEquals("length of read-only alias after trim()", 2, test1.length());
    assertEquals("length of terminated buffer of read-only alias + trim()",
                 2, u_strlen(test1.getTerminatedBuffer()));
}