int GetTLSCipherBits(rfbClient* client)
{
    SSL *ssl = (SSL *)(client->tlsSession);

    const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl);

    return SSL_CIPHER_get_bits(cipher, NULL);
}