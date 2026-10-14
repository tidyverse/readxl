#pragma once

// Minimal one-shot SHA-1 and SHA-2 (FIPS 180-4), as needed for decrypting
// ECMA-376 encrypted Office documents. Inputs are always small, so there is no
// streaming interface.

#include <cstdint>
#include <string>

namespace sha {

inline uint32_t rotl32(uint32_t x, int n) { return (x << n) | (x >> (32 - n)); }
inline uint32_t rotr32(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
inline uint64_t rotr64(uint64_t x, int n) { return (x >> n) | (x << (64 - n)); }

// Append padding and the big-endian bit length. `block` is 64 or 128 bytes.
inline std::string pad(const std::string& msg, size_t block) {
  size_t len_bytes = block / 8;
  std::string buf = msg + '\x80';
  while ((buf.size() + len_bytes) % block != 0) {
    buf += '\0';
  }
  uint64_t bits = static_cast<uint64_t>(msg.size()) * 8;
  for (int i = static_cast<int>(len_bytes) - 1; i >= 0; i--) {
    buf += static_cast<char>(i < 8 ? (bits >> (8 * i)) & 0xff : 0);
  }
  return buf;
}

template <typename T>
T load_be(const std::string& s, size_t pos) {
  T x = 0;
  for (size_t i = 0; i < sizeof(T); i++) {
    x = (x << 8) | static_cast<uint8_t>(s[pos + i]);
  }
  return x;
}

template <typename T>
std::string store_be(const T* h, size_t out_len) {
  std::string out;
  for (size_t i = 0; i < out_len; i++) {
    size_t shift = 8 * (sizeof(T) - 1 - i % sizeof(T));
    out += static_cast<char>((h[i / sizeof(T)] >> shift) & 0xff);
  }
  return out;
}

inline std::string sha1(const std::string& msg) {
  uint32_t h[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
  std::string buf = pad(msg, 64);
  for (size_t off = 0; off < buf.size(); off += 64) {
    uint32_t w[80];
    for (int t = 0; t < 16; t++) {
      w[t] = load_be<uint32_t>(buf, off + 4 * t);
    }
    for (int t = 16; t < 80; t++) {
      w[t] = rotl32(w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16], 1);
    }
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
    for (int t = 0; t < 80; t++) {
      uint32_t f, k;
      if (t < 20) {
        f = (b & c) | (~b & d);
        k = 0x5a827999;
      } else if (t < 40) {
        f = b ^ c ^ d;
        k = 0x6ed9eba1;
      } else if (t < 60) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8f1bbcdc;
      } else {
        f = b ^ c ^ d;
        k = 0xca62c1d6;
      }
      uint32_t tmp = rotl32(a, 5) + f + e + k + w[t];
      e = d;
      d = c;
      c = rotl32(b, 30);
      b = a;
      a = tmp;
    }
    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
  }
  return store_be(h, 20);
}

inline std::string sha256(const std::string& msg) {
  static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
  uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                   0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  std::string buf = pad(msg, 64);
  for (size_t off = 0; off < buf.size(); off += 64) {
    uint32_t w[64];
    for (int t = 0; t < 16; t++) {
      w[t] = load_be<uint32_t>(buf, off + 4 * t);
    }
    for (int t = 16; t < 64; t++) {
      uint32_t s0 = rotr32(w[t - 15], 7) ^ rotr32(w[t - 15], 18) ^ (w[t - 15] >> 3);
      uint32_t s1 = rotr32(w[t - 2], 17) ^ rotr32(w[t - 2], 19) ^ (w[t - 2] >> 10);
      w[t] = w[t - 16] + s0 + w[t - 7] + s1;
    }
    uint32_t v[8];
    for (int i = 0; i < 8; i++) v[i] = h[i];
    for (int t = 0; t < 64; t++) {
      uint32_t s1 = rotr32(v[4], 6) ^ rotr32(v[4], 11) ^ rotr32(v[4], 25);
      uint32_t ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
      uint32_t t1 = v[7] + s1 + ch + k[t] + w[t];
      uint32_t s0 = rotr32(v[0], 2) ^ rotr32(v[0], 13) ^ rotr32(v[0], 22);
      uint32_t maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
      for (int i = 7; i > 0; i--) v[i] = v[i - 1];
      v[4] += t1;
      v[0] = t1 + s0 + maj;
    }
    for (int i = 0; i < 8; i++) h[i] += v[i];
  }
  return store_be(h, 32);
}

// SHA-512, or SHA-384 if `out_len` is 48
inline std::string sha512(const std::string& msg, size_t out_len = 64) {
  static const uint64_t k[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
    0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
    0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
    0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
    0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
    0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
    0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
    0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
    0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
    0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
    0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
    0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
    0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
    0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
    0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
    0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
    0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
    0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
    0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
    0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
    0x5fcb6fab3ad6faec, 0x6c44198c4a475817};
  static const uint64_t iv512[8] = {
    0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b,
    0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f,
    0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
  static const uint64_t iv384[8] = {
    0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17,
    0x152fecd8f70e5939, 0x67332667ffc00b31, 0x8eb44a8768581511,
    0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4};
  uint64_t h[8];
  for (int i = 0; i < 8; i++) h[i] = out_len == 48 ? iv384[i] : iv512[i];
  std::string buf = pad(msg, 128);
  for (size_t off = 0; off < buf.size(); off += 128) {
    uint64_t w[80];
    for (int t = 0; t < 16; t++) {
      w[t] = load_be<uint64_t>(buf, off + 8 * t);
    }
    for (int t = 16; t < 80; t++) {
      uint64_t s0 = rotr64(w[t - 15], 1) ^ rotr64(w[t - 15], 8) ^ (w[t - 15] >> 7);
      uint64_t s1 = rotr64(w[t - 2], 19) ^ rotr64(w[t - 2], 61) ^ (w[t - 2] >> 6);
      w[t] = w[t - 16] + s0 + w[t - 7] + s1;
    }
    uint64_t v[8];
    for (int i = 0; i < 8; i++) v[i] = h[i];
    for (int t = 0; t < 80; t++) {
      uint64_t s1 = rotr64(v[4], 14) ^ rotr64(v[4], 18) ^ rotr64(v[4], 41);
      uint64_t ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
      uint64_t t1 = v[7] + s1 + ch + k[t] + w[t];
      uint64_t s0 = rotr64(v[0], 28) ^ rotr64(v[0], 34) ^ rotr64(v[0], 39);
      uint64_t maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
      for (int i = 7; i > 0; i--) v[i] = v[i - 1];
      v[4] += t1;
      v[0] = t1 + s0 + maj;
    }
    for (int i = 0; i < 8; i++) h[i] += v[i];
  }
  return store_be(h, out_len);
}

} // namespace sha
