TEST(RegexMatchExpression, MalformedRegexAcceptedButMatchesNothing) {
    RegexMatchExpression regex("a", "[(*ACCEPT)", "");
    ASSERT_FALSE(regex.matchesBSON(BSON("a"
                                        << "")));
    ASSERT_FALSE(regex.matchesBSON(BSON("a"
                                        << "[")));
}