/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 *
 */
/*
 * @file        drm_testcore.cpp
 * @author      Kyungwook Tak(k.tak@samsung.com)
 * @version     1.0
 * @brief       Test drm library internal core codes
 */
#include "TADC_Util.h"

#include <iostream>
#include <cstring>

bool tc_base64(void)
{
	unsigned char in[16] = {
		0x12, 0x93, 0xe8, 0xf1, 0x28, 0xb3, 0xff, 0x85,
		0x83, 0xab, 0x8c, 0x1f, 0x7f, 0x9e, 0x15, 0x22
	};

	char *encoded = Base64Encode(in, sizeof(in));
	std::cout << "encoded: " << encoded << std::endl;

	int len = 0;
	unsigned char *decoded = Base64Decode(encoded, &len);
	if (len <= 0
			|| static_cast<size_t>(len) != sizeof(in)
			|| memcmp(in, decoded, sizeof(in)) != 0) {
		std::cerr << "input and decoded base64 is different!!" << std::endl;
		return false;
	}

	return true;
}

void test_drm_core(void)
{
	tc_base64();
}
