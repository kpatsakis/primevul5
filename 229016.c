TEST(ModMatchExpression, ZeroDivisor) {
    ASSERT_THROWS_CODE(ModMatchExpression mod("", 0, 1), AssertionException, ErrorCodes::BadValue);
}