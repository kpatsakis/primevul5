START_TEST(SecureChannel_sendAsymmetricOPNMessage_sentDataIsValid) {
    UA_OpenSecureChannelResponse dummyResponse;
    createDummyResponse(&dummyResponse);

    /* Enable encryption for the SecureChannel */
#ifdef UA_ENABLE_ENCRYPTION
    testChannel.securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;
#else
    testChannel.securityMode = UA_MESSAGESECURITYMODE_NONE;
#endif

    UA_UInt32 requestId = UA_UInt32_random();

    UA_StatusCode retval =
        UA_SecureChannel_sendAsymmetricOPNMessage(&testChannel, requestId, &dummyResponse,
                                                  &UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE]);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "Expected function to succeed");

    size_t offset = 0;
    UA_TcpMessageHeader header;
    UA_TcpMessageHeader_decodeBinary(&sentData, &offset, &header);
    UA_UInt32 secureChannelId;
    UA_UInt32_decodeBinary(&sentData, &offset, &secureChannelId);

    UA_AsymmetricAlgorithmSecurityHeader asymSecurityHeader;
    UA_AsymmetricAlgorithmSecurityHeader_decodeBinary(&sentData, &offset, &asymSecurityHeader);

    ck_assert_msg(UA_ByteString_equal(&testChannel.securityPolicy->policyUri,
                                      &asymSecurityHeader.securityPolicyUri),
                  "Expected securityPolicyUri to be equal to the one used by the secureChannel");

#ifdef UA_ENABLE_ENCRYPTION
    ck_assert_msg(UA_ByteString_equal(&dummyCertificate, &asymSecurityHeader.senderCertificate),
                  "Expected the certificate to be equal to the one used  by the secureChannel");

    UA_ByteString thumbPrint = {20, testChannel.remoteCertificateThumbprint};
    ck_assert_msg(UA_ByteString_equal(&thumbPrint,
                                      &asymSecurityHeader.receiverCertificateThumbprint),
                  "Expected receiverCertificateThumbprint to be equal to the one set "
                  "in the secureChannel");

    /* Dummy encryption */
    for(size_t i = offset; i < header.messageSize; ++i) {
        sentData.data[i] = (UA_Byte)((sentData.data[i] - 1) % (UA_BYTE_MAX + 1));
    }
#endif

    UA_SequenceHeader sequenceHeader;
    UA_SequenceHeader_decodeBinary(&sentData, &offset, &sequenceHeader);
    ck_assert_msg(sequenceHeader.requestId == requestId, "Expected requestId to be %i but was %i",
                  requestId,
                  sequenceHeader.requestId);

    UA_NodeId requestTypeId;
    UA_NodeId_decodeBinary(&sentData, &offset, &requestTypeId);
    ck_assert_msg(UA_NodeId_equal(&UA_TYPES[UA_TYPES_OPENSECURECHANNELRESPONSE].binaryEncodingId, &requestTypeId), "Expected nodeIds to be equal");

    UA_OpenSecureChannelResponse sentResponse;
    UA_OpenSecureChannelResponse_decodeBinary(&sentData, &offset, &sentResponse);

    ck_assert_msg(memcmp(&sentResponse, &dummyResponse, sizeof(UA_OpenSecureChannelResponse)) == 0,
                  "Expected the sent response to be equal to the one supplied to the send function");

#ifdef UA_ENABLE_ENCRYPTION
    UA_Byte paddingByte = sentData.data[offset];
    size_t paddingSize = (size_t)paddingByte;

    for(size_t i = 0; i <= paddingSize; ++i) {
        ck_assert_msg(sentData.data[offset + i] == paddingByte,
                      "Expected padding byte %i to be %i but got value %i",
                      (int)i, paddingByte, sentData.data[offset + i]);
    }

    ck_assert_msg(sentData.data[offset + paddingSize + 1] == '*', "Expected first byte of signature");
#endif

    UA_AsymmetricAlgorithmSecurityHeader_clear(&asymSecurityHeader);
    UA_SequenceHeader_clear(&sequenceHeader);
    UA_OpenSecureChannelResponse_clear(&sentResponse);
} END_TEST