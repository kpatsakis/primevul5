START_TEST(SecureChannel_sendSymmetricMessage_invalidParameters) {
    // initialize dummy message
    UA_ReadRequest dummyMessage;
    UA_ReadRequest_init(&dummyMessage);
    UA_DataType dummyType = UA_TYPES[UA_TYPES_READREQUEST];

    UA_StatusCode retval = UA_SecureChannel_sendSymmetricMessage(NULL, 42, UA_MESSAGETYPE_MSG,
                                                                 &dummyMessage, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_HEL, &dummyMessage, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_ACK, &dummyMessage, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_ERR, &dummyMessage, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_OPN, &dummyMessage, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_MSG, NULL, &dummyType);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");

    retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42,
                                                   UA_MESSAGETYPE_MSG, &dummyMessage, NULL);
    ck_assert_msg(retval != UA_STATUSCODE_GOOD, "Expected failure");
} END_TEST