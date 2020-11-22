#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include "gtest/gtest.h"
#include "ACSVM/String.hpp"

using ACSVM::StrHash;
using ACSVM::String;
using ACSVM::StringData;
using ACSVM::StringTable;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::unordered_set;

TEST(ACSVMString, StrHash)
{
    ASSERT_NE(StrHash("Michael", 7), StrHash("michael", 7));
    ASSERT_EQ(StrHash("Michael", 7), StrHash("Michael"));
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

TEST(ACSVMString, StringTable)
{
    StringTable table;
    {
        // Test the "none" string
        const String &none = table.getNone();

        ASSERT_EQ(none.lock, 0);
        // none.ref is undefined at this moment

        ASSERT_EQ(none.idx, 0);
        ASSERT_EQ(none.len0, 0);
        ASSERT_EQ(none.get(0), 0);

        ASSERT_STREQ(none.str, "");
        ASSERT_EQ(none.len, 0);
        ASSERT_EQ(none.hash, StrHash(""));
    }

    {
        // Test the add-by-access
        String &string1 = table[{"Michael", 7}];
        String &string2 = table[{"Bolton", 6}];
        String &string3 = table[{"Designer", 6}];   // cut off
        ASSERT_EQ(table.size(), 3);
        ASSERT_EQ(table[0], string1);
        ASSERT_EQ(table[1], string2);
        ASSERT_EQ(table[2], string3);
        ASSERT_EQ(&table[0], &string1);
        ASSERT_EQ(&table[1], &string2);
        ASSERT_EQ(&table[2], &string3);

        // Test "idx" info
        ASSERT_EQ(string1.idx, 0);
        ASSERT_EQ(string2.idx, 1);
        ASSERT_EQ(string3.idx, 2);
        // Check that "String::get" works
        for(int i = 0; i < 6; ++i)
            ASSERT_EQ(string3.get(i), "Designer"[i]);
        // Check that it honours the cut-off
        ASSERT_EQ(string3.get(6), 0);
        ASSERT_EQ(string3.get(7), 0);
        // NOTE: len0 will be correct, and input string truncated correctly
        ASSERT_EQ(string3.len0, 6);
        ASSERT_STREQ(string3.str, "Design");

        // Check that it's safe to overflow
        ASSERT_EQ(string3.get(100), 0);

        // Test the garbage collection
        table.collectBegin();
        ASSERT_FALSE(string1.ref);
        ASSERT_FALSE(string2.ref);
        ASSERT_FALSE(string3.ref);
        // Keep reference to one
        string2.ref = true;
        table.collectEnd();
        ASSERT_EQ(table.size(), 1);
        ASSERT_EQ(table[0], table.getNone());
        ASSERT_EQ(table[1], string2);
        ASSERT_EQ(table[2], table.getNone());

        // Check that the reference is still active
        ASSERT_TRUE(string2.ref);

        // Add two other strings
        String &string1B = table[{"Richard", 7}];
        String &string3B = table[{"Gibson", 6}];

        // Check their presence in either position 0 or 2
        ASSERT_EQ(table.size(), 3);
        // Any of the following positions must be satisfied (we don't care about ordering now)
        if(table[0] == string1B)
            ASSERT_EQ(table[2], string3B);
        else if(table[2] == string1B)
            ASSERT_EQ(table[0], string3B);
        else
            ASSERT_TRUE(false);

        // Check that the unremoved string was left alone
        ASSERT_STREQ(table[1].str, "Bolton");
        // Add a lock for later checking
        ++table[1].lock;

        // Check that it's safe to go overflow
        ASSERT_EQ(table[100], table.getNone());

        // Saving the state
        stringstream stream;
        table.saveState(stream);

        // Test clearing the table also removes the referenced string
        table.clear();
        ASSERT_EQ(table.size(), 0);

        // Test loading the state to another table works
        StringTable newTable;
        newTable.loadState(stream);
        ASSERT_STREQ(newTable[1].str, "Bolton");
        unordered_set<string> otherStrings;
        otherStrings.insert(newTable[0].str);
        otherStrings.insert(newTable[2].str);
        ASSERT_TRUE(otherStrings.count("Richard"));
        ASSERT_TRUE(otherStrings.count("Gibson"));
        ASSERT_EQ(newTable[1].lock, 1);

        // Test that the lock is equally checked like the ref
        newTable.collectBegin();
        newTable.collectEnd();
        ASSERT_EQ(newTable.size(), 1);
        ASSERT_STREQ(newTable[1].str, "Bolton");
        --newTable[1].lock;
        ASSERT_EQ(newTable[1].lock, 0);
    }
}
