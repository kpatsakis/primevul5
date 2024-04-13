START_TEST(SecureChannel_sendAsymmetricOPNMessage_SecurityModeNone) {
    // Configure our channel correctly for OPN messages and setup dummy message
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);
    testChannel.securityMode = UA_MESSAGESECURITYMODE_NONE;

    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, &dummyResponse,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected function to succeed");
    ck_assert_msg(!fCalled.asym_enc, "Message encryption was called but should not have been");
    ck_assert_msg(!fCalled.asym_sign, "Message signing was called but should not have been");
}