#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>

SSL_CTX	*init_ssl()
{
	SSL_library_init();
	OpenSSL_add_all_algorithms(); // init crypto lib used by ssl
	SSL_load_error_strings(); // give precise error descriptions
	return SSL_CTX_new(TLS_client_method());
}

SSL	*create_ssl_connection(SSL_CTX *ctx, int sock_fd, char *hostname)
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
	if (SSL_set_fd(ssl, sock_fd) != 1)
		goto error_after_ssl_creation;
	if (SSL_set_tlsext_host_name(ssl, hostname) != 1)
		goto error_after_ssl_creation;
	if (SSL_connect(ssl) <= 0)
		goto error_after_ssl_creation;
	return ssl;
error_after_ssl_creation:
	ERR_print_errors_fp(stderr);
	SSL_free(ssl);
	return NULL;
}
