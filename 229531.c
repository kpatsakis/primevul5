UnicodeStringTest::TestStartsWithAndEndsWithNulTerminated() {
    UnicodeString test("abcde");
    const UChar ab[] = { 0x61, 0x62, 0 };
    const UChar de[] = { 0x64, 0x65, 0 };
    assertTrue("abcde.startsWith(ab, -1)", test.startsWith(ab, -1));
    assertTrue("abcde.startsWith(ab, 0, -1)", test.startsWith(ab, 0, -1));
    assertTrue("abcde.endsWith(de, -1)", test.endsWith(de, -1));
    assertTrue("abcde.endsWith(de, 0, -1)", test.endsWith(de, 0, -1));
}