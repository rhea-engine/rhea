#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Url.h"

namespace psio {

	// test for empty url
	TEST(URLTest, Empty) {
		Url url;
		EXPECT_TRUE(url.IsEmpty());
		EXPECT_FALSE(url.IsValid());
	}

	// test for creating urls via constructor
	TEST(URLTest, Basic) {

		Url url("http://www.example.org/path/to/file");
		EXPECT_FALSE(url.IsEmpty());
		EXPECT_TRUE(url.IsValid());
		EXPECT_EQ("www.example.org", url.Hostname());
		EXPECT_EQ("http", url.Scheme());
		EXPECT_EQ("/path/to/file", url.Path());
		EXPECT_EQ("", url.Password());
		EXPECT_EQ("", url.Username());
		EXPECT_EQ(80, url.Port());
		EXPECT_FALSE(url.IsCryptographic());
	}

	// test for creating https urls via constructor
	TEST(URLTest, DetectsHttps) {

		Url url("https://www.example.org/path/to/file");
		EXPECT_FALSE(url.IsEmpty());
		EXPECT_TRUE(url.IsValid());
		EXPECT_EQ("www.example.org", url.Hostname());
		EXPECT_EQ("https", url.Scheme());
		EXPECT_EQ("/path/to/file", url.Path());
		EXPECT_EQ("", url.Password());
		EXPECT_EQ("", url.Username());
		EXPECT_EQ(443, url.Port());
		EXPECT_TRUE(url.IsCryptographic());
	}

	// test for creating a full url via constructor without path
	TEST(URLTest, DetectsFullCryptoNoPath) {

		Url url("https://user:pass@www.example.org:4443/");
		EXPECT_FALSE(url.IsEmpty());
		EXPECT_TRUE(url.IsValid());
		EXPECT_EQ("www.example.org", url.Hostname());
		EXPECT_EQ("https", url.Scheme());
		EXPECT_EQ("/", url.Path());
		EXPECT_EQ("user", url.Username());
		EXPECT_EQ("pass", url.Password());
		EXPECT_EQ(4443, url.Port());
		EXPECT_TRUE(url.IsCryptographic());
	}

	// test for creating a full url via constructor without path
	TEST(URLTest, DetectsFullCryptoWithPath) {

		Url url("https://user:pass@www.example.org:4443/path/to/file");
		EXPECT_FALSE(url.IsEmpty());
		EXPECT_TRUE(url.IsValid());
		EXPECT_EQ("www.example.org", url.Hostname());
		EXPECT_EQ("https", url.Scheme());
		EXPECT_EQ("/path/to/file", url.Path());
		EXPECT_EQ("user", url.Username());
		EXPECT_EQ("pass", url.Password());
		EXPECT_EQ(4443, url.Port());
		EXPECT_TRUE(url.IsCryptographic());
	}

	// test for empty path
	TEST(URLTest, EmptyPath) {

		Url url("http://www.example.org");
		EXPECT_TRUE(url.IsValid());
		EXPECT_EQ("/", url.Path());
	}
}