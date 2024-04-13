process_callback(void *application, UA_SecureChannel *channel,
                 UA_MessageType messageType, UA_UInt32 requestId,
                 UA_ByteString *message) {
    ck_assert_ptr_ne(message, NULL);
    ck_assert_ptr_ne(application, NULL);
    if(message == NULL || application == NULL)
        return UA_STATUSCODE_BADINTERNALERROR;
    ck_assert_uint_ne(message->length, 0);
    ck_assert_ptr_ne(message->data, NULL);
    int *chunks_processed = (int *)application;
    ++*chunks_processed;
    return UA_STATUSCODE_GOOD;
}