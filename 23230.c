START_TEST(SecureChannel_sendAsymmetricOPNMessage_SecurityModeSignAndEncrypt) {
    // Configure our channel correctly for OPN messages and setup dummy message
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);

    testChannel.securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;
    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, &dummyResponse,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected function to succeed");
    ck_assert_msg(fCalled.asym_enc, "Expected message to have been encrypted but it was not");
    ck_assert_msg(fCalled.asym_sign, "Expected message to have been signed but it was not");
}END_TEST