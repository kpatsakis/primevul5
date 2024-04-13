testSuite_SecureChannel(void) {
    Suite *s = suite_create("SecureChannel");

    TCase *tc_initAndDelete = tcase_create("Initialize and delete Securechannel");
    tcase_add_checked_fixture(tc_initAndDelete, setup_funcs_called, teardown_funcs_called);
    tcase_add_checked_fixture(tc_initAndDelete, setup_key_sizes, teardown_key_sizes);
    tcase_add_test(tc_initAndDelete, SecureChannel_initAndDelete);
    suite_add_tcase(s, tc_initAndDelete);

    TCase *tc_sendAsymmetricOPNMessage = tcase_create("Test sendAsymmetricOPNMessage function");
    tcase_add_checked_fixture(tc_sendAsymmetricOPNMessage, setup_funcs_called, teardown_funcs_called);
    tcase_add_checked_fixture(tc_sendAsymmetricOPNMessage, setup_key_sizes, teardown_key_sizes);
    tcase_add_checked_fixture(tc_sendAsymmetricOPNMessage, setup_secureChannel, teardown_secureChannel);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_withoutConnection);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_invalidParameters);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_SecurityModeInvalid);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_SecurityModeNone);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_sentDataIsValid);
#ifdef UA_ENABLE_ENCRYPTION
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_SecurityModeSign);
    tcase_add_test(tc_sendAsymmetricOPNMessage, SecureChannel_sendAsymmetricOPNMessage_SecurityModeSignAndEncrypt);
    tcase_add_test(tc_sendAsymmetricOPNMessage,
                   Securechannel_sendAsymmetricOPNMessage_extraPaddingPresentWhenKeyLargerThan2048Bits);
#endif
    suite_add_tcase(s, tc_sendAsymmetricOPNMessage);

    TCase *tc_sendSymmetricMessage = tcase_create("Test sendSymmetricMessage function");
    tcase_add_checked_fixture(tc_sendSymmetricMessage, setup_funcs_called, teardown_funcs_called);
    tcase_add_checked_fixture(tc_sendSymmetricMessage, setup_key_sizes, teardown_key_sizes);
    tcase_add_checked_fixture(tc_sendSymmetricMessage, setup_secureChannel, teardown_secureChannel);
    tcase_add_test(tc_sendSymmetricMessage, SecureChannel_sendSymmetricMessage);
    tcase_add_test(tc_sendSymmetricMessage, SecureChannel_sendSymmetricMessage_invalidParameters);
    tcase_add_test(tc_sendSymmetricMessage, SecureChannel_sendSymmetricMessage_modeNone);
#ifdef UA_ENABLE_ENCRYPTION
    tcase_add_test(tc_sendSymmetricMessage, SecureChannel_sendSymmetricMessage_modeSign);
    tcase_add_test(tc_sendSymmetricMessage, SecureChannel_sendSymmetricMessage_modeSignAndEncrypt);
#endif
    suite_add_tcase(s, tc_sendSymmetricMessage);

    TCase *tc_processBuffer = tcase_create("Test chunk assembly");
    tcase_add_checked_fixture(tc_processBuffer, setup_funcs_called, teardown_funcs_called);
    tcase_add_checked_fixture(tc_processBuffer, setup_key_sizes, teardown_key_sizes);
    tcase_add_checked_fixture(tc_processBuffer, setup_secureChannel, teardown_secureChannel);
    tcase_add_test(tc_processBuffer, SecureChannel_assemblePartialChunks);
    suite_add_tcase(s, tc_processBuffer);

    return s;
}