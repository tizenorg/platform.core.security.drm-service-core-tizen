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

#include <vector>
#include <openssl/dh.h>
#include <tzplatform_config.h>

#define RIURL "http://appdrm.test.com/"

#define RO_ISSUER_SIGNER_KEY_FILE tzplatform_mkpath(TZ_SYS_DATA, "drm_test/Key/ro_test_signer.key")

#define STR_PLACE_HOLDER_CID        "PLACE_HOLDER_FOR_CID"
#define STR_PLACE_HOLDER_DUID       "PLACE_HOLDER_FOR_DUID"
#define STR_PLACE_HOLDER_SIGNATURE  "PLACE_HOLDER_FOR_SIGNATURE"

#define MAX_CERT_SIZE      (1024*128)

#define DRMTEST_SUCCESS       ( 0)
#define DRMTEST_ERR_CRYPTO    (-1)
#define DRMTEST_ERR_IO        (-2)
#define DRMTEST_ERR_MEMORY    (-3)
#define DRMTEST_ERR_TIZDRM    (-4)
#define DRMTEST_ERR_PARAM     (-5)

#define DRMTEST_IDENTICAL     ( 0)
#define DRMTEST_NOTIDENTICAL  (-1)

#define DRMTEST_TEST_ROOT_CERT (1)
#define DRMTEST_REAL_ROOT_CERT (2)

#define EVP_SUCCESS 1	// DO NOTCHANGE THIS VALUE

int generate_purchase_response(char** purchase_response_buff, char** req_id);

int generate_right_object_request(const char* license_response_buff);

int get_dh_key_from_ro_request(const char* ro_request_buff,
                              char** dh_key_p, char** dh_key_g, char** dh_key_a);

int generate_right_object_response(const char* dh_key_p, const char* dh_key_g, const char* dh_key_a,
                              const char* req_id, const char* cid, const char* ro_template_path,
                              const char* duid, char** ro_response_buff);
int is_identical_files(const char* file1, const char* file2, int* identical);

int switch_root_cert(int target_cert_type);


void _base64_encode(const unsigned char* input, int len, char** output);
void _base64_decode(const char* input, unsigned char** output, int* out_len);
char* _replace_all(char *s, const char *olds, const char *news);
int _read_text_file(const char* path, char** output);

int _create_dh_key(const char* dh_key_p_hex, const char* dh_key_g_hex, DH** ppkey);
int _free_dh_key(DH* pkey);
int _get_dh_hex_pubkey(const DH* pkey, char** dh_pubkey);
int _get_dh_shared_secret_key(const char* dh_hex_pubkey, DH *pkey,
                                unsigned char** dh_shared_secret_key, int *dh_sec_key_len);

int _create_right_object_without_signature(const char* ro_template_path, const char* cid, const char* duid,
                               char** ro_buff);
int _create_ro_signature(const char* ro_buff, const char* signer_prikey_path, char** signature);
int _add_signature_to_ro(const char* ro_buff, const char* signature, char** ro_with_signature);
int _encrypt_ro_with_dh_sec_key(const char* ro_with_signature, 
                                const unsigned char* dh_secret_key, const int dh_sec_key_len, 
                                char **encrypted_ro);

int _create_response_data_in_ro_response(const char* reqid, const char* encrypted_ro, const char* dh_pubkey,
                                char **response_data); 

int _create_time_stamp(const unsigned char* dh_secret_key, char** time_stamp);


using Binary = std::vector<unsigned char>;
Binary _read_ro_file(const char *filename);
