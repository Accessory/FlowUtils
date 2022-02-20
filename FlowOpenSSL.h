#pragma once

#include <iostream>
#include <memory>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <string>
#include <vector>

namespace FlowOpenSSL {

struct KeyPair {
  std::string publicKey;
  std::string privateKey;
};

inline static std::vector<unsigned char>
sha1(const std::vector<unsigned char> &input) {

  unsigned char digest[SHA_DIGEST_LENGTH];
  std::vector<unsigned char> rtn;

  SHA_CTX context;
  if (!SHA1_Init(&context))
    return rtn;

  if (!SHA1_Update(&context, (unsigned char *)input.data(), input.size()))
    return rtn;

  if (!SHA1_Final(digest, &context))
    return rtn;

  rtn.assign(digest, digest + SHA_DIGEST_LENGTH);

  return rtn;
}

inline static std::vector<unsigned char> sha1(const std::string &input) {

  unsigned char digest[SHA_DIGEST_LENGTH];
  std::vector<unsigned char> rtn;

  SHA_CTX context;
  if (!SHA1_Init(&context))
    return rtn;

  if (!SHA1_Update(&context, (unsigned char *)input.c_str(), input.size()))
    return rtn;

  if (!SHA1_Final(digest, &context))
    return rtn;

  rtn.assign(digest, digest + SHA_DIGEST_LENGTH);

  return rtn;
}

inline static std::vector<unsigned char> sha256(const std::string &input) {

  unsigned char digest[SHA256_DIGEST_LENGTH];
  std::vector<unsigned char> rtn;

  SHA256_CTX context;
  if (!SHA256_Init(&context))
    return rtn;

  if (!SHA256_Update(&context, (unsigned char *)input.c_str(), input.size()))
    return rtn;

  if (!SHA256_Final(digest, &context))
    return rtn;

  rtn.assign(digest, digest + SHA256_DIGEST_LENGTH);

  return rtn;
}

inline static std::vector<unsigned char>
sha256(const std::vector<unsigned char> &input) {

  unsigned char digest[SHA256_DIGEST_LENGTH];
  std::vector<unsigned char> rtn;

  SHA256_CTX context;
  if (!SHA256_Init(&context))
    return rtn;

  if (!SHA256_Update(&context, input.data(), input.size()))
    return rtn;

  if (!SHA256_Final(digest, &context))
    return rtn;

  rtn.assign(digest, digest + SHA256_DIGEST_LENGTH);

  return rtn;
}

inline static std::string toString(BIO *bio) {
  std::string rtn;
  char buffer[2048];
  size_t read;
  BIO_read_ex(bio, buffer, sizeof(buffer), &read);
  while (read > 0) {
    rtn.insert(rtn.end(), buffer, buffer + read);
    BIO_read_ex(bio, buffer, sizeof(buffer), &read);
  }
  return rtn;
}

inline static void printBio(BIO *bio) {
  char buffer[1024];
  while (BIO_read(bio, buffer, 1024) > 0) {
    std::cout << buffer;
  }
}

inline static KeyPair generateKeyPair(size_t keyLength = 4096) {
  using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
  using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
  using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
  using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

  RSA_ptr rsa(RSA_new(), ::RSA_free);
  BN_ptr bn(BN_new(), ::BN_free);

  BIO_FILE_ptr pem1(BIO_new_file("rsa-public-1.pem", "w"), ::BIO_free);
  BIO_FILE_ptr pem2(BIO_new_file("rsa-public-2.pem", "w"), ::BIO_free);
  BIO_FILE_ptr pem3(BIO_new_file("rsa-private-1.pem", "w"), ::BIO_free);
  BIO_FILE_ptr pem4(BIO_new_file("rsa-private-2.pem", "w"), ::BIO_free);
  BIO_FILE_ptr pem5(BIO_new_file("rsa-private-3.pem", "w"), ::BIO_free);
  BIO_FILE_ptr der1(BIO_new_file("rsa-public.der", "w"), ::BIO_free);
  BIO_FILE_ptr der2(BIO_new_file("rsa-private.der", "w"), ::BIO_free);

  BIO_FILE_ptr strpri(BIO_new(BIO_s_mem()), ::BIO_free);
  BIO_FILE_ptr strpub(BIO_new(BIO_s_mem()), ::BIO_free);

  int rc = BN_set_word(bn.get(), RSA_F4);
  rc = RSA_generate_key_ex(rsa.get(), keyLength, bn.get(), NULL);
  if (rc != 1)
    return {};

  EVP_KEY_ptr pkey(EVP_PKEY_new(), ::EVP_PKEY_free);
  rc = EVP_PKEY_set1_RSA(pkey.get(), rsa.get());
  if (rc != 1)
    return {};

  rc = i2d_RSAPublicKey_bio(der1.get(), rsa.get());
  if (rc != 1)
    return {};

  // Write public key in PKCS PEM
  // rc = PEM_write_bio_RSAPublicKey(pem1.get(), rsa.get());

  // Write public key in Traditional PEM
  // rc = PEM_write_bio_PUBKEY(pem2.get(), pkey.get());

  //////////

  // Write private key in ASN.1/DER
  // rc = i2d_RSAPrivateKey_bio(der2.get(), rsa.get());

  // Write private key in PKCS PEM.
  // rc = PEM_write_bio_PrivateKey(pem3.get(), pkey.get(), NULL, NULL, 0, NULL,
  //                               NULL);

  // Write private key in PKCS PEM
  // rc = PEM_write_bio_PKCS8PrivateKey(pem4.get(), pkey.get(), NULL, NULL, 0,
  //                                    NULL, NULL);

  // Write private key in Traditional PEM
  // rc = PEM_write_bio_RSAPrivateKey(pem5.get(), rsa.get(), NULL, NULL, 0,
  // NULL,
  //                                  NULL);

  rc = PEM_write_bio_PrivateKey(strpri.get(), pkey.get(), NULL, NULL, 0, NULL,
                                NULL);
  if (rc != 1)
    return {};
  rc = PEM_write_bio_PUBKEY(strpub.get(), pkey.get());
  if (rc != 1)
    return {};

  // std::cout << "Public Key: " << std::endl;
  // std::cout << toString(strpub.get()) << std::endl;

  // std::cout << "Private Key: " << std::endl;
  // std::cout << toString(strpri.get()) << std::endl;

  // return {toString(strpri.get()), toString(strpub.get())};
  return {toString(strpub.get()), toString(strpri.get())};
}

inline static std::string encodeMessage(const std::string &to_enc,
                                        const std::string &public_key) {
  using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
  // using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
  using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
  using EVP_PKEY_CTX_ptr =
      std::unique_ptr<EVP_PKEY_CTX, decltype(&::EVP_PKEY_CTX_free)>;
  using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

  auto my_deleter = [](unsigned char *p) {
    CRYPTO_free(p, OPENSSL_FILE, OPENSSL_LINE);
  };
  using OUT_ptr = std::unique_ptr<unsigned char, decltype(my_deleter)>;

  BIO_FILE_ptr strpub(BIO_new(BIO_s_mem()), ::BIO_free);
  int rc = BIO_write(strpub.get(), public_key.c_str(), public_key.size());
  // if (rc != 1)
  // return "";

  // RSA_ptr rsa(PEM_read_bio_RSA_PUBKEY(strpub.get(), NULL, NULL, NULL),
  // &::RSA_free);
  auto rsa = PEM_read_bio_RSA_PUBKEY(strpub.get(), NULL, NULL, NULL);
  EVP_KEY_ptr public_evp_key(EVP_PKEY_new(), ::EVP_PKEY_free);
  EVP_PKEY_assign_RSA(public_evp_key.get(), rsa);

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(public_evp_key.get(), NULL),
                       ::EVP_PKEY_CTX_free);

  rc = EVP_PKEY_encrypt_init(ctx.get());
  if (rc != 1)
    return "";

  rc = EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING);
  if (rc != 1)
    return "";

  size_t out_len = 0;
  rc = EVP_PKEY_encrypt(ctx.get(), NULL, &out_len,
                        reinterpret_cast<const unsigned char *>(to_enc.data()),
                        to_enc.size());
  if (rc != 1)
    return "";

  // unsigned char* out = reinterpret_cast<unsigned char
  // *>(OPENSSL_malloc(out_len));
  OUT_ptr out(reinterpret_cast<unsigned char *>(OPENSSL_malloc(out_len)),
              my_deleter);
  rc = EVP_PKEY_encrypt(ctx.get(), out.get(), &out_len,
                        reinterpret_cast<const unsigned char *>(to_enc.c_str()),
                        to_enc.size());
  if (rc != 1)
    return "";

  return std::string(reinterpret_cast<char *>(out.get()), out_len);
}

inline static std::string decodeMessage(const std::string &to_dec,
                                        const std::string &private_key) {
  using BN_ptr = std::unique_ptr<BIGNUM, decltype(&::BN_free)>;
  // using RSA_ptr = std::unique_ptr<RSA, decltype(&::RSA_free)>;
  using EVP_KEY_ptr = std::unique_ptr<EVP_PKEY, decltype(&::EVP_PKEY_free)>;
  using EVP_PKEY_CTX_ptr =
      std::unique_ptr<EVP_PKEY_CTX, decltype(&::EVP_PKEY_CTX_free)>;
  using BIO_FILE_ptr = std::unique_ptr<BIO, decltype(&::BIO_free)>;

  auto my_deleter = [](unsigned char *p) {
    CRYPTO_free(p, OPENSSL_FILE, OPENSSL_LINE);
  };
  using OUT_ptr = std::unique_ptr<unsigned char, decltype(my_deleter)>;

  BIO_FILE_ptr strpriv(BIO_new(BIO_s_mem()), ::BIO_free);
  int rc = BIO_write(strpriv.get(), private_key.c_str(), private_key.size());
  // if (rc != 1)
  // return "";

  // RSA_ptr rsa(PEM_read_bio_RSA_PUBKEY(strpriv.get(), NULL, NULL, NULL),
  // &::RSA_free);
  auto rsa = PEM_read_bio_RSAPrivateKey(strpriv.get(), NULL, NULL, NULL);
  EVP_KEY_ptr public_evp_key(EVP_PKEY_new(), ::EVP_PKEY_free);
  EVP_PKEY_assign_RSA(public_evp_key.get(), rsa);

  EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(public_evp_key.get(), NULL),
                       ::EVP_PKEY_CTX_free);

  rc = EVP_PKEY_decrypt_init(ctx.get());
  if (rc != 1)
    return "";

  rc = EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING);
  if (rc != 1)
    return "";

  size_t out_len = 0;
  rc = EVP_PKEY_decrypt(ctx.get(), NULL, &out_len,
                        reinterpret_cast<const unsigned char *>(to_dec.data()),
                        to_dec.size());
  if (rc != 1)
    return "";

  // unsigned char* out = reinterpret_cast<unsigned char
  // *>(OPENSSL_malloc(out_len));
  OUT_ptr out(reinterpret_cast<unsigned char *>(OPENSSL_malloc(out_len)),
              my_deleter);
  rc = EVP_PKEY_decrypt(ctx.get(), out.get(), &out_len,
                        reinterpret_cast<const unsigned char *>(to_dec.data()),
                        to_dec.size());
  if (rc != 1)
    return "";

  return std::string(reinterpret_cast<char *>(out.get()), out_len);
}
}; // namespace FlowOpenSSL
