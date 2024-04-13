UnicodeStringTest::TestAppendable() {
    UnicodeString dest;
    SimpleAppendable app(dest);
    doTestAppendable(dest, app);
}