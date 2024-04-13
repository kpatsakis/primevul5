createDummyResponse(UA_OpenSecureChannelResponse *response) {
    UA_OpenSecureChannelResponse_init(response);
    memset(response, 0, sizeof(UA_OpenSecureChannelResponse));
}