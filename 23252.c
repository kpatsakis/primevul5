START_TEST(SecureChannel_sendSymmetricMessage_modeNone) {
    // initialize dummy message
    UA_ReadRequest dummyMessage;
    UA_ReadRequest_init(&dummyMessage);
    UA_DataType dummyType = UA_TYPES[UA_TYPES_READREQUEST];

    testChannel.securityMode = UA_MESSAGESECURITYMODE_NONE;

    UA_StatusCode retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42, UA_MESSAGETYPE_MSG,
                                                                 &dummyMessage, &dummyType);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_msg(!fCalled.sym_sign, "Expected message to not have been signed");
    ck_assert_msg(!fCalled.sym_enc, "Expected message to not have been encrypted");
} END_TEST