TEST(RegexMatchExpression, RegexOptionsStringCannotContainEmbeddedNullByte) {
    {
        const auto embeddedNull = "a\0b"_sd;
        ASSERT_THROWS_CODE(RegexMatchExpression regex("path", "pattern", embeddedNull),
                           AssertionException,
                           ErrorCodes::BadValue);
    }

    {
        const auto singleNullByte = "\0"_sd;
        ASSERT_THROWS_CODE(RegexMatchExpression regex("path", "pattern", singleNullByte),
                           AssertionException,
                           ErrorCodes::BadValue);
    }

    {
        const auto leadingNullByte = "\0bbbb"_sd;
        ASSERT_THROWS_CODE(RegexMatchExpression regex("path", "pattern", leadingNullByte),
                           AssertionException,
                           ErrorCodes::BadValue);
    }

    {
        const auto trailingNullByte = "bbbb\0"_sd;
        ASSERT_THROWS_CODE(RegexMatchExpression regex("path", "pattern", trailingNullByte),
                           AssertionException,
                           ErrorCodes::BadValue);
    }
}