START_TEST(SecureChannel_sendAsymmetricOPNMessage_withoutConnection) {
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);
    testChannel.securityMode = UA_MESSAGESECURITYMODE_NONE;

    // Remove connection to provoke error
    UA_Connection_detachSecureChannel(testChannel.connection);
    testChannel.connection = NULL;

    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, 42, &dummyResponse,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);

    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure without a connection");
}END_TEST