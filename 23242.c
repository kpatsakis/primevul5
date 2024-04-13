START_TEST(SecureChannel_assemblePartialChunks) {
    int chunks_processed = 0;
    UA_ByteString buffer = UA_BYTESTRING_NULL;

    buffer.data = (UA_Byte *)"HELF \x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00"
                             "\x10\x00\x00\x00@\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff";
    buffer.length = 32;

    UA_StatusCode retval = UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 1);

    buffer.length = 16;

    UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 1);

    buffer.data = &buffer.data[16];
    UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 2);

    buffer.data = (UA_Byte *)"HELF \x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00"
                             "\x10\x00\x00\x00@\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"
                             "HELF \x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00"
                             "\x10\x00\x00\x00@\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff"
                             "HELF \x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00"
                             "\x10\x00\x00\x00@\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff";
    buffer.length = 48;

    UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 3);

    buffer.data = &buffer.data[48];
    buffer.length = 32;

    UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 4);

    buffer.data = &buffer.data[32];
    buffer.length = 16;
    UA_SecureChannel_processBuffer(&testChannel, &chunks_processed, process_callback, &buffer);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected success");
    ck_assert_int_eq(chunks_processed, 5);
} END_TEST