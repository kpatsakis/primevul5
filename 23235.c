START_TEST(SecureChannel_initAndDelete) {
    TestingPolicy(&dummyPolicy, dummyCertificate, &fCalled, &keySizes);
    UA_StatusCode retval;

    UA_SecureChannel channel;
    UA_SecureChannel_init(&channel, &UA_ConnectionConfig_default);
    retval = UA_SecureChannel_setSecurityPolicy(&channel, &dummyPolicy, &dummyCertificate);

    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected StatusCode to be good");
    ck_assert_msg(channel.state == UA_SECURECHANNELSTATE_FRESH, "Expected state to be new/fresh");
    ck_assert_msg(fCalled.newContext, "Expected newContext to have been called");
    ck_assert_msg(fCalled.makeCertificateThumbprint,
                  "Expected makeCertificateThumbprint to have been called");
    ck_assert_msg(channel.securityPolicy == &dummyPolicy, "SecurityPolicy not set correctly");

    UA_SecureChannel_close(&channel);
    ck_assert_msg(fCalled.deleteContext, "Expected deleteContext to have been called");

    dummyPolicy.clear(&dummyPolicy);
}END_TEST