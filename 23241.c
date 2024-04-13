teardown_secureChannel(void) {
    UA_SecureChannel_close(&testChannel);
    dummyPolicy.clear(&dummyPolicy);
    testingConnection.close(&testingConnection);
}