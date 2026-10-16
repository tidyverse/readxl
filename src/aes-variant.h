/* tiny-AES-c fixes the AES key size at compile time. This header compiles it
 * for the key size selected by the including file (aes128.c, aes192.c,
 * aes256.c), renaming the public symbols so all variants can be linked into
 * one library, and adds a single decrypt entry point:
 *
 *   readxl_aesNNN_decrypt(key, iv, buf, len)
 *
 * which decrypts `buf` in place with AES-CBC, or with AES-ECB if `iv` is NULL.
 * `len` must be a multiple of 16. */

#include <stddef.h>
#include <stdint.h>

#define AES_CAT_(a, b) a##b
#define AES_CAT(a, b) AES_CAT_(a, b)
#define AES_NAME(x) AES_CAT(AES_PREFIX, x)

#define AES_init_ctx AES_NAME(init_ctx)
#define AES_init_ctx_iv AES_NAME(init_ctx_iv)
#define AES_ctx_set_iv AES_NAME(ctx_set_iv)
#define AES_ECB_encrypt AES_NAME(ECB_encrypt)
#define AES_ECB_decrypt AES_NAME(ECB_decrypt)
#define AES_CBC_encrypt_buffer AES_NAME(CBC_encrypt_buffer)
#define AES_CBC_decrypt_buffer AES_NAME(CBC_decrypt_buffer)
#define CTR 0

#include "tinyaes/aes.c"

void AES_NAME(decrypt)(const uint8_t* key, const uint8_t* iv, uint8_t* buf,
                       size_t len) {
  struct AES_ctx ctx;
  if (iv) {
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, buf, len);
  } else {
    AES_init_ctx(&ctx, key);
    for (size_t i = 0; i + AES_BLOCKLEN <= len; i += AES_BLOCKLEN) {
      AES_ECB_decrypt(&ctx, buf + i);
    }
  }
}
