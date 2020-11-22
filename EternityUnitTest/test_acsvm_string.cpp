#include "gtest/gtest.h"
#include "ACSVM/String.hpp"

using ACSVM::StrHash;
using ACSVM::StringData;

TEST(ACSVMString, StrHash)
{
    ASSERT_NE(StrHash("Michael", 7), StrHash("michael", 7));
    ASSERT_NE(StrHash("Michael", 7), StrHash("michael", 6));
    ASSERT_EQ(StrHash("Michael", 7), StrHash("Michael", 7));
    ASSERT_EQ(StrHash("Michael", 0), StrHash("Jackson", 0));
}

TEST(ACSVMString, StringData)
{
    static const char data[] = "Michael";

    StringData fullStringData(data, data + 7);
    ASSERT_STREQ(fullStringData.str, data);
    ASSERT_EQ(fullStringData.len, 7);
    ASSERT_EQ(fullStringData.hash, StrHash(data, 7));

    {
        StringData stringData(data, 6);
        ASSERT_EQ(stringData.len, 6);
        ASSERT_EQ(stringData.hash, StrHash(data, 6));

        static char otherData[] = "Michael";
        ASSERT_NE(otherData, data);
        StringData secondData(otherData, 6);
        ASSERT_EQ(secondData, stringData);
    }
    {
        StringData arbitrary(data, 7, 123);
        ASSERT_STREQ(arbitrary.str, data);
        ASSERT_EQ(arbitrary.len, 7);
        ASSERT_EQ(arbitrary.hash, 123);
        ASSERT_FALSE(arbitrary == fullStringData);
    }
}
