

#include "ecryptfs_kernel.h"

void ecryptfs_dump_auth_tok(struct ecryptfs_auth_tok *auth_tok)
{
	char salt[ECRYPTFS_SALT_SIZE * 2 + 1];
	char sig[ECRYPTFS_SIG_SIZE_HEX + 1];

	ecryptfs_printk(KERN_DEBUG, "Auth tok at mem loc [%p]:\n",
			auth_tok);
	if (auth_tok->flags & ECRYPTFS_PRIVATE_KEY) {
		ecryptfs_printk(KERN_DEBUG, " * private key type\n");
	} else {
		ecryptfs_printk(KERN_DEBUG, " * passphrase type\n");
		ecryptfs_to_hex(salt, auth_tok->token.password.salt,
				ECRYPTFS_SALT_SIZE);
		salt[ECRYPTFS_SALT_SIZE * 2] = '\0';
		ecryptfs_printk(KERN_DEBUG, " * salt = [%s]\n", salt);
		if (auth_tok->token.password.flags &
		    ECRYPTFS_PERSISTENT_PASSWORD) {
			ecryptfs_printk(KERN_DEBUG, " * persistent\n");
		}
		memcpy(sig, auth_tok->token.password.signature,
		       ECRYPTFS_SIG_SIZE_HEX);
		sig[ECRYPTFS_SIG_SIZE_HEX] = '\0';
		ecryptfs_printk(KERN_DEBUG, " * signature = [%s]\n", sig);
	}
	ecryptfs_printk(KERN_DEBUG, " * session_key.flags = [0x%x]\n",
			auth_tok->session_key.flags);
	if (auth_tok->session_key.flags
	    & ECRYPTFS_USERSPACE_SHOULD_TRY_TO_DECRYPT)
		ecryptfs_printk(KERN_DEBUG,
				" * Userspace decrypt request set\n");
	if (auth_tok->session_key.flags
	    & ECRYPTFS_USERSPACE_SHOULD_TRY_TO_ENCRYPT)
		ecryptfs_printk(KERN_DEBUG,
				" * Userspace encrypt request set\n");
	if (auth_tok->session_key.flags & ECRYPTFS_CONTAINS_DECRYPTED_KEY) {
		ecryptfs_printk(KERN_DEBUG, " * Contains decrypted key\n");
		ecryptfs_printk(KERN_DEBUG,
				" * session_key.decrypted_key_size = [0x%x]\n",
				auth_tok->session_key.decrypted_key_size);
		ecryptfs_printk(KERN_DEBUG, " * Decrypted session key "
				"dump:\n");
		if (ecryptfs_verbosity > 0)
			ecryptfs_dump_hex(auth_tok->session_key.decrypted_key,
					  ECRYPTFS_DEFAULT_KEY_BYTES);
	}
	if (auth_tok->session_key.flags & ECRYPTFS_CONTAINS_ENCRYPTED_KEY) {
		ecryptfs_printk(KERN_DEBUG, " * Contains encrypted key\n");
		ecryptfs_printk(KERN_DEBUG,
				" * session_key.encrypted_key_size = [0x%x]\n",
				auth_tok->session_key.encrypted_key_size);
		ecryptfs_printk(KERN_DEBUG, " * Encrypted session key "
				"dump:\n");
		if (ecryptfs_verbosity > 0)
			ecryptfs_dump_hex(auth_tok->session_key.encrypted_key,
					  auth_tok->session_key.
					  encrypted_key_size);
	}
}

void ecryptfs_dump_hex(char *data, int bytes)
{
	int i = 0;
	int add_newline = 1;

	if (ecryptfs_verbosity < 1)
		return;
	if (bytes != 0) {
		printk(KERN_DEBUG "0x%.2x.", (unsigned char)data[i]);
		i++;
	}
	while (i < bytes) {
		printk("0x%.2x.", (unsigned char)data[i]);
		i++;
		if (i % 16 == 0) {
			printk("\n");
			add_newline = 0;
		} else
			add_newline = 1;
	}
	if (add_newline)
		printk("\n");
}

