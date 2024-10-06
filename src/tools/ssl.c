#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>

SSL_CTX	*create_ctx_ssl()
{
	SSL_library_init();
	SSL_load_error_strings(); // give precise error descriptions
	OpenSSL_add_ssl_algorithms(); // init crypto lib used by ssl
	return SSL_CTX_new(TLS_client_method());
}

SSL	*create_ssl_connection(SSL_CTX *ctx, int sock_fd)
{
	SSL	*ssl;

	if (!ctx) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	ssl = SSL_new(ctx);
	if (!ssl) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	SSL_set_fd(ssl, sock_fd);
	if (SSL_connect(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		return NULL;
	}
	return ssl;
}
