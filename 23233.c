START_TEST(SecureChannel_sendAsymmetricOPNMessage_SecurityModeInvalid) {
    // Configure our channel correctly for OPN messages and setup dummy message
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);

    testChannel.securityMode = UA_MESSAGESECURITYMODE_INVALID;

    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, &dummyResponse,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);
    ck_assert_msg(retval == UA_STATUSCODE_BADSECURITYMODEREJECTED,
                  "Expected SecurityMode rejected error");
}