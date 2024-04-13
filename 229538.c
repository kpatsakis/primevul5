UnicodeStringTest::TestUnicodeStringImplementsAppendable() {
    UnicodeString dest;
    UnicodeStringAppendable app(dest);
    doTestAppendable(dest, app);
}