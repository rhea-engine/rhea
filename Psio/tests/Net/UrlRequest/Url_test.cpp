/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Psio/Net/UrlRequest/Url.h>

namespace psio {

	// test for empty url
	TEST(URLTest, Empty) {
		Url url;
		EXPECT_TRUE(url.isEmpty());
		EXPECT_FALSE(url.isValid());
	}

	// test for creating urls via constructor
	TEST(URLTest, Basic) {

		Url url("http://www.example.org/path/to/file");
		EXPECT_FALSE(url.isEmpty());
		EXPECT_TRUE(url.isValid());
		EXPECT_EQ("www.example.org", url.hostname());
		EXPECT_EQ("http", url.scheme());
		EXPECT_EQ("/path/to/file", url.path());
		EXPECT_EQ("", url.password());
		EXPECT_EQ("", url.username());
		EXPECT_EQ(80, url.port());
		EXPECT_FALSE(url.isCryptographic());
	}

	// test for creating https urls via constructor
	TEST(URLTest, DetectsHttps) {

		Url url("https://www.example.org/path/to/file");
		EXPECT_FALSE(url.isEmpty());
		EXPECT_TRUE(url.isValid());
		EXPECT_EQ("www.example.org", url.hostname());
		EXPECT_EQ("https", url.scheme());
		EXPECT_EQ("/path/to/file", url.path());
		EXPECT_EQ("", url.password());
		EXPECT_EQ("", url.username());
		EXPECT_EQ(443, url.port());
		EXPECT_TRUE(url.isCryptographic());
	}

	// userinfo is not supported and must make the URL invalid
	TEST(URLTest, RejectsUserInfoNoPath) {

		Url url("https://user:pass@www.example.org:4443/");
		EXPECT_FALSE(url.isEmpty());
		EXPECT_FALSE(url.isValid());
	}

	// userinfo is not supported and must make the URL invalid
	TEST(URLTest, RejectsUserInfoWithPath) {

		Url url("https://user:pass@www.example.org:4443/path/to/file");
		EXPECT_FALSE(url.isEmpty());
		EXPECT_FALSE(url.isValid());
	}

	// test for empty path
	TEST(URLTest, EmptyPath) {

		Url url("http://www.example.org");
		EXPECT_TRUE(url.isValid());
		EXPECT_EQ("", url.path());
		EXPECT_EQ("/", url.requestTarget());
	}
}