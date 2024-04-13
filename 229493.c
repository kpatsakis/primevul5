UnicodeStringTest::TestNameSpace() {
    // Provoke name collision unless the UnicodeString macros properly
    // qualify the icu::UnicodeString class.
    using namespace bogus;

    // Use all UnicodeString macros from unistr.h.
    icu::UnicodeString s1=icu::UnicodeString("abc", 3, US_INV);
    icu::UnicodeString s2=UNICODE_STRING("def", 3);
    icu::UnicodeString s3=UNICODE_STRING_SIMPLE("ghi");

    // Make sure the compiler does not optimize away instantiation of s1, s2, s3.
    icu::UnicodeString s4=s1+s2+s3;
    if(s4.length()!=9) {
        errln("Something wrong with UnicodeString::operator+().");
    }
}