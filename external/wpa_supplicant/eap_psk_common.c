
#include "includes.h"

#include "common.h"
#include "aes_wrap.h"
#include "eap_defs.h"
#include "eap_psk_common.h"

#define aes_block_size 16


void eap_psk_key_setup(const u8 *psk, u8 *ak, u8 *kdk)
{
	os_memset(ak, 0, aes_block_size);
	aes_128_encrypt_block(psk, ak, ak);
	os_memcpy(kdk, ak, aes_block_size);
	ak[aes_block_size - 1] ^= 0x01;
	kdk[aes_block_size - 1] ^= 0x02;
	aes_128_encrypt_block(psk, ak, ak);
	aes_128_encrypt_block(psk, kdk, kdk);
}


void eap_psk_derive_keys(const u8 *kdk, const u8 *rand_p, u8 *tek, u8 *msk,
			 u8 *emsk)
{
	u8 hash[aes_block_size];
	u8 counter = 1;
	int i;

	aes_128_encrypt_block(kdk, rand_p, hash);

	hash[aes_block_size - 1] ^= counter;
	aes_128_encrypt_block(kdk, hash, tek);
	hash[aes_block_size - 1] ^= counter;
	counter++;

	for (i = 0; i < EAP_MSK_LEN / aes_block_size; i++) {
		hash[aes_block_size - 1] ^= counter;
		aes_128_encrypt_block(kdk, hash, &msk[i * aes_block_size]);
		hash[aes_block_size - 1] ^= counter;
		counter++;
	}

	for (i = 0; i < EAP_EMSK_LEN / aes_block_size; i++) {
		hash[aes_block_size - 1] ^= counter;
		aes_128_encrypt_block(kdk, hash, &emsk[i * aes_block_size]);
		hash[aes_block_size - 1] ^= counter;
		counter++;
	}
}
