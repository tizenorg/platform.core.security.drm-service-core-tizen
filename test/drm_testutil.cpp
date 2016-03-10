/*
 * Copyright (c) 2000-2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/sha.h>

#include <TADC_IF.h>
#include <drm-tizen-apps.h>
#include <TADC_Util.h>

#include "drm_testutil.h"

#define _INITIALIZED 0
#define _UNINITIALIZED -1

using Binary = std::vector<unsigned char>;

static unsigned char g_baAESKey[32] =
{
    0xf8, 0x87, 0x0a, 0xc5, 0xd3, 0x6d, 0x44, 0x49, 0x03, 0x9f, 0xbd, 0x1e, 0xa8, 0x2f, 0xf6, 0xc3,
    0xdf, 0x3b, 0x02, 0x13, 0x58, 0x1b, 0x12, 0x30, 0x1c, 0xd7, 0xad, 0xa5, 0x1f, 0x5d, 0x01, 0x33
};


int __initialized = _UNINITIALIZED;

void __init_crypto()
{
    if(__initialized != _INITIALIZED ) {
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        __initialized = _INITIALIZED;
    }
}

int __get_random_bytes(char* output, int random_len)
{
    FILE *fp = fopen("/dev/urandom", "r");
    auto size = fread(output, 1, random_len, fp);

	fclose(fp);

	if (size != static_cast<size_t>(random_len))
		return DRMTEST_ERR_IO;

    return DRMTEST_SUCCESS;
}

int __file_copy(const char* target_path, const char* source_path)
{
    int ret = DRMTEST_SUCCESS;
    FILE *source = NULL;
    FILE *target = NULL;
    size_t l1,l2;
    unsigned char buffer[8192];

    source = fopen(source_path, "r");
    if(source == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    target = fopen(target_path, "w");
    if(target == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    while((l1 = fread(buffer, 1, sizeof buffer, source)) > 0) {
        l2 = fwrite(buffer, 1, l1, target);
        if(l2 < l1) {
            if(ferror(target)) {
                ret = DRMTEST_ERR_IO;
                goto error;
            }
        }
    }

error:
    if(source != NULL)
        fclose(source);
    if(target != NULL)
        fclose(target);
    return ret;
}

void _base64_encode(const unsigned char* input, int length, char** output)
{
    *output = Base64Encode((unsigned char*)input, length);
}

void _base64_decode(const char* input, unsigned char** output, int* out_len)
{
    *output = Base64Decode((char*)input, out_len);
}

char* _replace_all(char *s, const char *olds, const char *news)
{
    char *result=NULL, *sr=NULL;
    size_t i, count = 0;
    size_t oldlen = strlen(olds); if (oldlen < 1) return s;
    size_t newlen = strlen(news);

    if (newlen != oldlen) {
        for (i = 0; s[i] != '\0';) {
          if (memcmp(&s[i], olds, oldlen) == 0) count++, i += oldlen;
          else i++;
        }
    } else i = strlen(s);

    result = (char *) malloc(i + 1 + count * (newlen - oldlen));
    if (result == NULL) return NULL;

    sr = result;
    while (*s) {
        if (memcmp(s, olds, oldlen) == 0) {
            memcpy(sr, news, newlen);
            sr += newlen;
            s  += oldlen;
        } else *sr++ = *s++;
    }
    *sr = '\0';

    return result;
}

Binary _read_ro_file(const char *filename)
{
    try {
        std::ifstream is(filename, std::ifstream::binary);
        if (!is || !is.is_open())
            return Binary();

        is.seekg(0, is.end);
        int length = is.tellg();
        is.seekg(0, is.beg);

        Binary buffer(length + 1); /* for null-terminated */

        is.read(reinterpret_cast<char *>(buffer.data()), length);
        if (!is)
            return Binary();

        return buffer;
    } catch (...) {
        std::cerr << "Failed to read ro file: " << filename << std::endl;
        return Binary();
    }
}

int _create_dh_key(const char* dh_key_p_hex, const char* dh_key_g_hex, DH** ppkey)
{
    int ret = DRMTEST_SUCCESS;
    DH *pDH = NULL;

    if ((pDH = DH_new()) == NULL) {
        printf("...FAIL: DH_new() error");
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    BN_hex2bn(&(pDH->p), dh_key_p_hex);
    BN_hex2bn(&(pDH->g), dh_key_g_hex);

    /* Set a to run with normal modexp and b to use constant time */
    pDH->flags &= ~DH_FLAG_NO_EXP_CONSTTIME;

    // Generate DH Key
    if (!DH_generate_key(pDH)) {
        printf("...FAIL: DH_generate_key");
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    *ppkey = pDH;
error:
    if(ret != DRMTEST_SUCCESS && pDH != NULL)
        DH_free(pDH);

    return ret;
}

int _free_dh_key(DH* pkey)
{
    if(pkey != NULL)
        DH_free(pkey);
    return DRMTEST_SUCCESS;
}


int _get_dh_hex_pubkey(const DH* pkey, char** dh_pubkey)
{
    *dh_pubkey = BN_bn2hex(pkey->pub_key);
    return DRMTEST_SUCCESS;
}

int _get_dh_shared_secret_key(const char* dh_hex_pubkey, DH *pkey,
                              unsigned char** dh_shared_secret_key, int *dh_sec_key_len)
{
    int ret = DRMTEST_SUCCESS;

    BIGNUM  *pPubKey = NULL;
    unsigned char *secret_key_buff = NULL;
    unsigned char tmp_buff[DH_size(pkey)] = {0,};

    BN_hex2bn(&pPubKey, dh_hex_pubkey);

    if(DH_compute_key(tmp_buff, pPubKey, pkey) < 0) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    secret_key_buff = (unsigned char *) malloc(DH_size(pkey)/2);
    if(secret_key_buff == NULL) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }
    memset(secret_key_buff, 0, DH_size(pkey)/2);

    printf("APP_DRM TEST: shared secret : ");
    for(int i=0; i<(DH_size(pkey)/2); i++) {
        secret_key_buff[i] = tmp_buff[i * 2] ^ tmp_buff[(i * 2) + 1];
        printf("%02x", secret_key_buff[i]);
    }
    printf("\n");

    *dh_shared_secret_key = secret_key_buff;
    *dh_sec_key_len = DH_size(pkey)/2;
error:
    if(pPubKey != NULL)
        BN_free(pPubKey);
    if(ret != DRMTEST_SUCCESS && secret_key_buff != NULL)
        free(secret_key_buff);

    return ret;
}


int _create_right_object_without_signature(const char* ro_template_path, const char* cid, const char* duid,
                                char** ro_buff)
{
    int ret = DRMTEST_SUCCESS;
    char *cid_filled = NULL;
    char *duid_filled = NULL;

    auto buf = _read_ro_file(ro_template_path);

    cid_filled = _replace_all(reinterpret_cast<char *>(buf.data()), STR_PLACE_HOLDER_CID, cid);
    duid_filled = _replace_all(cid_filled, STR_PLACE_HOLDER_DUID, duid);

    *ro_buff = duid_filled;

    if (cid_filled != NULL)
        free(cid_filled);

    if (ret != DRMTEST_SUCCESS && duid_filled != NULL)
        free(duid_filled);

    return ret;
}

int _create_ro_signature(const char* ro_buff, const char* signer_prikey_path, char** signature)
{
    int ret = DRMTEST_SUCCESS;

    EVP_PKEY* pkey = NULL;
    RSA*      prsa = NULL;
    const char* END_STR = "</CertificateChain>";
    const char* tmp_end_hash_input = NULL;
    int hash_input_size = 0;
    unsigned char hash_value[20];
    unsigned char sig_value[MAX_CERT_SIZE]; // enough buffer
    unsigned int sig_len = 0;
    char* b64_sig_value = NULL;

    FILE *file = NULL;

    // get private key
    file = fopen(signer_prikey_path, "r");
    if(file == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    pkey = PEM_read_PrivateKey(file, &pkey, NULL, NULL);
    if(pkey == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    prsa = EVP_PKEY_get1_RSA(pkey);
    if(prsa == NULL) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    // get hash input size
    tmp_end_hash_input = strstr(ro_buff, END_STR);
    hash_input_size = (tmp_end_hash_input - ro_buff) + strlen(END_STR);

    // get hash value
    SHA_CTX     alginfo;
    SHA1_Init(&alginfo);
    SHA1_Update(&alginfo, ro_buff, hash_input_size);
    SHA1_Final(hash_value, &alginfo);

    // get signature value
    if( 1 != RSA_sign(NID_sha1, hash_value, 20, sig_value, &sig_len, prsa) ) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    // convert to base64 string
    _base64_encode(sig_value, (int) sig_len, &b64_sig_value);

    *signature = b64_sig_value;
error:
    if(file != NULL)
        fclose(file);
    if(pkey != NULL)
        EVP_PKEY_free(pkey);
    if(ret != DRMTEST_SUCCESS && b64_sig_value != NULL)
        free(b64_sig_value);

    return ret;
}

int _add_signature_to_ro(const char* ro_buff, const char* signature, char** ro_with_signature)
{
    int ret = DRMTEST_SUCCESS;
    char *buff = NULL;
    buff = _replace_all((char*)ro_buff, STR_PLACE_HOLDER_SIGNATURE, signature);
    *ro_with_signature = buff;
    return ret;
}

int _encrypt_ro_with_dh_sec_key(const char* ro_with_signature,
                                const unsigned char* dh_secret_key, const int dh_sec_key_len,
                                char **encrypted_ro)
{
    int ret = DRMTEST_SUCCESS;
    TADC_U8     key[16] = {0, };
    TADC_U8     iv[16] = {0, };
    int encrypted_len = 0;
    unsigned char encrypted_buff[MAX_CERT_SIZE] = {0, };;

    (void) dh_sec_key_len; // to prevent unused varialbe error

    TADC_IF_MemCpy(key, dh_secret_key, 16);
    TADC_IF_MemCpy(iv, (dh_secret_key+16), 16);
    ret = TADC_IF_AES_CTR(key, 16, iv, strlen(ro_with_signature), (unsigned char*)ro_with_signature,
                                    &encrypted_len, (unsigned char*)encrypted_buff);
    if(ret != 0) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    _base64_encode(encrypted_buff, encrypted_len, encrypted_ro);

error:
    return ret;
}

int _create_response_data_in_ro_response(const char* reqid, const char* encrypted_ro, const char* dh_pubkey,
                                char **response_data)
{
    int ret = DRMTEST_SUCCESS;
    char tmp_buff[MAX_CERT_SIZE] = {0,};
    unsigned char hashed_reqid[20]={0,};
    char hex_hashed_reqid[256] = {0, };
    unsigned char hash_value[20]={0,};
    int hmac_len = 0;
    unsigned char hmac[1024*10] = {0,};
    char* hmac_base64 = NULL;
    char* resp_data = NULL;

    // get hashed req_id
    SHA_CTX alginfoForReqId;
    SHA1_Init(&alginfoForReqId);
    SHA1_Update(&alginfoForReqId, reqid, strlen(reqid));
    SHA1_Final(hashed_reqid, &alginfoForReqId);

    for (size_t i = 0; i < sizeof(hashed_reqid); i++)
        sprintf(hex_hashed_reqid + i * 2, "%02x", hashed_reqid[i]);

    sprintf(tmp_buff, "reqid=%s;B=%s;license=%s", hex_hashed_reqid, dh_pubkey, encrypted_ro);

    // get hash value
    SHA_CTX     alginfo;
    SHA1_Init(&alginfo);
    SHA1_Update(&alginfo, tmp_buff, strlen(tmp_buff));
    SHA1_Final(hash_value, &alginfo);

    // encrypt hash value
    TADC_U8     key[16] = {0, };
    TADC_U8     iv[16] = {0, };

    TADC_IF_MemCpy(key, g_baAESKey, 16);
    TADC_IF_MemCpy(iv, (g_baAESKey+16), 16);
    ret = TADC_IF_AES_CTR(key, 16, iv, 20, hash_value, &hmac_len, hmac);
    if(ret != 0) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    // base64 encode
    _base64_encode(hmac, 20, &hmac_base64);

    // add hmac
    strncat(tmp_buff, ";hmac=", strlen(";hmac="));
    strncat(tmp_buff, hmac_base64, strlen(hmac_base64));

    // make return value
    resp_data = (char*) malloc(strlen(tmp_buff)+1);
    if(resp_data == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(resp_data, 0, strlen(tmp_buff)+1);
    strncpy(resp_data, tmp_buff, strlen(tmp_buff));

    *response_data = resp_data;

error:
    if(hmac_base64 != NULL)
        free(hmac_base64);
    if(ret != DRMTEST_SUCCESS && resp_data != NULL)
        free(resp_data);
    return ret;
}

int _create_time_stamp(const unsigned char* dh_secret_key, char** time_stamp)
{
    int ret = DRMTEST_SUCCESS;

    char tmp_time_buff[128] = {0,};
    unsigned char enc_time_buff[512] = {0,};
    char *time_base64 = NULL;
    int enc_time_buff_len = 0;
    time_t now = time(NULL);
    const struct tm* gt = gmtime(&now);

    sprintf(tmp_time_buff, "%d-%d-%dT%d:%d:00:Z",
                           gt->tm_year+1900, gt->tm_mon+1, gt->tm_mday,
                           gt->tm_hour+1, gt->tm_min+1);

    // encrypt time_stamp
    TADC_U8     key[16] = {0, };
    TADC_U8     iv[16] = {0, };

    TADC_IF_MemCpy(key, dh_secret_key, 16);
    TADC_IF_MemCpy(iv, (dh_secret_key+16), 16);
    ret = TADC_IF_AES_CTR(key, 16, iv, strlen(tmp_time_buff), (unsigned char*)tmp_time_buff,
                          &enc_time_buff_len, enc_time_buff);
    if(ret != 0) {
        ret = DRMTEST_ERR_CRYPTO;
        goto error;
    }

    // convert to base64
    _base64_encode(enc_time_buff, enc_time_buff_len, &time_base64);

    *time_stamp = time_base64;
error:
    if(ret != DRMTEST_SUCCESS && time_base64 != NULL)
        free(time_base64);
    return ret;

}

int generate_purchase_response(char** purchase_response_buff, char** req_id)
{
    int ret = DRMTEST_SUCCESS;
    char resp_buff[1024*5] = {0, };
    char *resp = NULL;
    char *rid = NULL;
    char random[64] = {0, };

    const char *format1 = "<?xml version=\"1.0\">\n";
    const char *format2 = "<response result=\"0\" message=\"\">\n";
    const char *format3 = "    <DRMType>%d</DRMType>\n";
    const char *format4 = "    <riurl>%s</riurl>\n";
    const char *format5 = "    <reqid>%s</reqid>\n";
    const char *format6 = "</response>";

    resp = (char*) malloc(sizeof(resp_buff));
    if(resp == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(resp, 0, sizeof(resp_buff));

    __get_random_bytes(random, sizeof(random));
    rid = (char*) malloc(1024);
    if(rid == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(rid, 0, 1024);

    for (size_t i = 0; i < sizeof(random); i++)
        sprintf(rid + i * 2, "%02x", random[i]);

    strncat(resp_buff, format1, strlen(format1));
    strncat(resp_buff, format2, strlen(format2));
    strncat(resp_buff, format3, strlen(format3));
    strncat(resp_buff, format4, strlen(format4));
    strncat(resp_buff, format5, strlen(format5));
    strncat(resp_buff, format6, strlen(format6));

    sprintf(resp, resp_buff, 1, RIURL, rid);

    *purchase_response_buff = resp;
    *req_id = rid;

error:
    if(ret != DRMTEST_SUCCESS && resp != NULL)
        free(resp);
    if(ret != DRMTEST_SUCCESS && rid != NULL)
        free(rid);

    return ret;
}

int generate_right_object_request(const char* license_response_buff)
{
    int ret = DRMTEST_SUCCESS;
    unsigned int  req_buff_len = 1024*5;
    char url_buff[1024] = {0, };
    unsigned int  url_buff_len = sizeof(url_buff);
    char *req_buff = NULL;

    req_buff = (char *)malloc(1024*5);
    if(req_buff == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(req_buff, 0, req_buff_len);

    ret = drm_tizen_generate_license_request(license_response_buff, strlen(license_response_buff),
                                             req_buff, &req_buff_len, url_buff, &url_buff_len);
    if(ret != 1) {
        ret = DRMTEST_ERR_TIZDRM;
        goto error;
    }

error:
    if(ret != DRMTEST_SUCCESS && req_buff != NULL)
        free(req_buff);
    return ret;
}


int get_dh_key_from_ro_request(const char* ro_request_buff,
                              char** dh_key_p, char** dh_key_g, char** dh_key_a)
{
    int ret = DRMTEST_SUCCESS;

    const char* PFX_P = "p=";
    const char* PFX_G = ";g=";
    const char* PFX_A = ";A=";
    const char* PFX_HMAC = ";hmac=";

    const char * idx_p = strstr(ro_request_buff, PFX_P);
    const char * idx_g = strstr(ro_request_buff, PFX_G);
    const char * idx_a = strstr(ro_request_buff, PFX_A);
    const char * idx_hmac = strstr(ro_request_buff, PFX_HMAC);

    int len_p = idx_g - idx_p - strlen(PFX_P);
    int len_g = idx_a - idx_g - strlen(PFX_G);
    int len_a = idx_hmac - idx_a - strlen(PFX_A);

    char* buff_p = NULL;
    char* buff_g = NULL;
    char* buff_a = NULL;

    buff_p = (char *)malloc(len_p + 1);
    if(buff_p == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(buff_p, 0, len_p + 1);
    strncpy(buff_p, idx_p + strlen(PFX_P), len_p);
    *dh_key_p = buff_p;

    buff_g = (char *)malloc(len_g + 1);
    if(buff_g == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(buff_g, 0, len_g + 1);
    strncpy(buff_g, idx_g + strlen(PFX_G), len_g);
    *dh_key_g = buff_g;

    buff_a = (char *)malloc(len_a + 1);
    if(buff_a == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(buff_a, 0, len_a + 1);
    strncpy(buff_a, idx_a + strlen(PFX_A), len_a);
    *dh_key_a = buff_a;

error:
    if(ret != DRMTEST_SUCCESS && buff_p != NULL)
        free(buff_p);
    if(ret != DRMTEST_SUCCESS && buff_g != NULL)
        free(buff_g);
    if(ret != DRMTEST_SUCCESS && buff_a != NULL)
        free(buff_a);

    return ret;
}

int generate_right_object_response(const char* dh_key_p, const char* dh_key_g, const char* dh_key_a,
                              const char* req_id, const char* cid, const char* ro_template_path,
                              const char* duid, char** ro_response_buff)
{
    int ret = DRMTEST_SUCCESS;

    DH* pkey = NULL;
    char* dh_pubkey = NULL;
    unsigned char* dh_shared_secret_key = NULL;
    int   dh_sec_key_len = 0;
    char* ro_buff = NULL;
    char* ro_signature = NULL;
    char* ro_with_signature = NULL;
    char* encrypted_ro = NULL;
    char* response_data = NULL;
    char* time_stamp = NULL;
    char* ro_resp_buff = NULL;
    int   ro_resp_buff_len = 0;

    const char *format = "<?xml version=\"1.0\">\n"
                         "<response result=\"0\" message=\"\">\n"
                         "    <responsedata>%s</responsedata>\n"
                         "    <timeStamp>%s</timeStamp>\n"
                         "</response>";

    ret = _create_dh_key(dh_key_p, dh_key_g, &pkey);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _get_dh_hex_pubkey(pkey, &dh_pubkey);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _get_dh_shared_secret_key(dh_key_a, pkey, &dh_shared_secret_key, &dh_sec_key_len);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _create_right_object_without_signature(ro_template_path, cid, duid, &ro_buff);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _create_ro_signature(ro_buff, RO_ISSUER_SIGNER_KEY_FILE, &ro_signature);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _add_signature_to_ro(ro_buff, ro_signature, &ro_with_signature);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }
    //printf("...right object:\n%s\n", ro_with_signature);

    ret = _encrypt_ro_with_dh_sec_key(ro_with_signature, dh_shared_secret_key, dh_sec_key_len,
                                      &encrypted_ro);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _create_response_data_in_ro_response(req_id, encrypted_ro, dh_pubkey, &response_data);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ret = _create_time_stamp( dh_shared_secret_key, &time_stamp);
    if(ret != DRMTEST_SUCCESS){
        goto error;
    }

    ro_resp_buff_len = strlen(format) + strlen(response_data) + strlen(time_stamp) + 1;
    ro_resp_buff = (char *) malloc(ro_resp_buff_len);
    if(ro_resp_buff == NULL) {
        ret = DRMTEST_ERR_MEMORY;
        goto error;
    }
    memset(ro_resp_buff, 0, ro_resp_buff_len);
    sprintf(ro_resp_buff, format, response_data, time_stamp);

    *ro_response_buff = ro_resp_buff;

error:
    if(pkey != NULL)
        _free_dh_key(pkey);
    if(dh_pubkey != NULL)
        free(dh_pubkey);
    if(dh_shared_secret_key != NULL)
        free(dh_shared_secret_key);
    if(ro_buff != NULL)
        free(ro_buff);
    if(ro_signature != NULL)
        free(ro_signature);
    if(ro_with_signature != NULL)
        free(ro_with_signature);
    if(encrypted_ro != NULL)
        free(encrypted_ro);
    if(response_data != NULL)
        free(response_data);
    if(time_stamp != NULL)
        free(time_stamp);
    if(ret != DRMTEST_SUCCESS && ro_resp_buff != NULL)
        free(ro_resp_buff);

    return ret;
}


int is_identical_files(const char* file1, const char* file2, int* identical)
{
    int ret = DRMTEST_SUCCESS;

    FILE *fp1 = NULL, *fp2 = NULL;
    int ch1, ch2;

    fp1 = fopen(file1, "r");
    if(fp1 == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    fp2 = fopen(file2, "r");
    if(fp2 == NULL) {
        ret = DRMTEST_ERR_IO;
        goto error;
    }

    ch1 = getc(fp1);
    ch2 = getc(fp2);
    while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }
    if(ch1 == ch2)
        *identical = DRMTEST_IDENTICAL;
    else
        *identical = DRMTEST_NOTIDENTICAL;

error:
    if(fp1 != NULL)
        fclose(fp1);
    if(fp2 != NULL)
        fclose(fp2);
    return ret;
}

