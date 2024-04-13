START_TEST(SecureChannel_sendAsymmetricOPNMessage_invalidParameters) {
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);

    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, NULL,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, &dummyResponse, NULL);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

}END_TEST