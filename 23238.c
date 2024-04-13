START_TEST(SecureChannel_sendSymmetricMessage) {
    // initialize dummy message
    UA_ReadRequest dummyMessage;
    UA_ReadRequest_init(&dummyMessage);
    UA_DataType dummyType = UA_TYPES[UA_TYPES_READREQUEST];

    UA_StatusCode retval = UA_SecureChannel_sendSymmetricMessage(&testChannel, 42, UA_MESSAGETYPE_MSG,
                                                                 &dummyMessage, &dummyType);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    // TODO: expand test
}