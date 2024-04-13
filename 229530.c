void UnicodeStringTest::TestUnicodeStringInsertAppendToSelf() {
    IcuTestErrorCode status(*this, "TestUnicodeStringAppendToSelf");

    // Test append operation
    UnicodeString str(u"foo ");
    str.append(str);
    str.append(str);
    str.append(str);
    assertEquals("", u"foo foo foo foo foo foo foo foo ", str);

    // Test append operation with readonly alias to start
    str = UnicodeString(TRUE, u"foo ", 4);
    str.append(str);
    str.append(str);
    str.append(str);
    assertEquals("", u"foo foo foo foo foo foo foo foo ", str);

    // Test append operation with aliased substring
    str = u"abcde";
    UnicodeString sub = str.tempSubString(1, 2);
    str.append(sub);
    assertEquals("", u"abcdebc", str);

    // Test append operation with double-aliased substring
    str = UnicodeString(TRUE, u"abcde", 5);
    sub = str.tempSubString(1, 2);
    str.append(sub);
    assertEquals("", u"abcdebc", str);

    // Test insert operation
    str = u"a-*b";
    str.insert(2, str);
    str.insert(4, str);
    str.insert(8, str);
    assertEquals("", u"a-a-a-a-a-a-a-a-*b*b*b*b*b*b*b*b", str);

    // Test insert operation with readonly alias to start
    str = UnicodeString(TRUE, u"a-*b", 4);
    str.insert(2, str);
    str.insert(4, str);
    str.insert(8, str);
    assertEquals("", u"a-a-a-a-a-a-a-a-*b*b*b*b*b*b*b*b", str);

    // Test insert operation with aliased substring
    str = u"abcde";
    sub = str.tempSubString(1, 3);
    str.insert(2, sub);
    assertEquals("", u"abbcdcde", str);

    // Test insert operation with double-aliased substring
    str = UnicodeString(TRUE, u"abcde", 5);
    sub = str.tempSubString(1, 3);
    str.insert(2, sub);
    assertEquals("", u"abbcdcde", str);
}