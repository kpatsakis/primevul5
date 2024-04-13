UnicodeStringTest::TestPrefixAndSuffix()
{
    UnicodeString test1("Now is the time for all good men to come to the aid of their country.");
    UnicodeString test2("Now");
    UnicodeString test3("country.");
    UnicodeString test4("count");

    if (!test1.startsWith(test2) || !test1.startsWith(test2, 0, test2.length())) {
        errln("startsWith() failed: \"" + test2 + "\" should be a prefix of \"" + test1 + "\".");
    }

    if (test1.startsWith(test3) ||
        test1.startsWith(test3.getBuffer(), test3.length()) ||
        test1.startsWith(test3.getTerminatedBuffer(), 0, -1)
    ) {
        errln("startsWith() failed: \"" + test3 + "\" shouldn't be a prefix of \"" + test1 + "\".");
    }

    if (test1.endsWith(test2)) {
        errln("endsWith() failed: \"" + test2 + "\" shouldn't be a suffix of \"" + test1 + "\".");
    }

    if (!test1.endsWith(test3)) { 
        errln("endsWith(test3) failed: \"" + test3 + "\" should be a suffix of \"" + test1 + "\".");
    }
    if (!test1.endsWith(test3, 0, INT32_MAX)) {
        errln("endsWith(test3, 0, INT32_MAX) failed: \"" + test3 + "\" should be a suffix of \"" + test1 + "\".");
    }

    if(!test1.endsWith(test3.getBuffer(), test3.length())) {
        errln("endsWith(test3.getBuffer(), test3.length()) failed: \"" + test3 + "\" should be a suffix of \"" + test1 + "\".");
    }
    if(!test1.endsWith(test3.getTerminatedBuffer(), 0, -1)) {
        errln("endsWith(test3.getTerminatedBuffer(), 0, -1) failed: \"" + test3 + "\" should be a suffix of \"" + test1 + "\".");
    }

    if (!test3.startsWith(test4)) {
        errln("endsWith(test4) failed: \"" + test4 + "\" should be a prefix of \"" + test3 + "\".");
    }

    if (test4.startsWith(test3)) {
        errln("startsWith(test3) failed: \"" + test3 + "\" shouldn't be a prefix of \"" + test4 + "\".");
    }
}