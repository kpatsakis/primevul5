TEST(RegexMatchExpression, TooLargePattern) {
    string tooLargePattern(50 * 1000, 'z');
    ASSERT_THROWS_CODE(RegexMatchExpression regex("a", tooLargePattern, ""),
                       AssertionException,
                       ErrorCodes::BadValue);
}