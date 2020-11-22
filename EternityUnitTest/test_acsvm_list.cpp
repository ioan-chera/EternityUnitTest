#include "gtest/gtest.h"
#include "ACSVM/List.hpp"

using ACSVM::ListLink;
using std::move;

TEST(ACSVMList, ListLink)
{
    {
        ListLink<int> link;
        ASSERT_EQ(link.obj, nullptr);
        ASSERT_EQ(link.prev, &link);
        ASSERT_EQ(link.next, &link);
    }
    {
        int n = 1;
        ListLink<int> head(&n);
        ASSERT_EQ(head.obj, &n);
        ASSERT_EQ(head.prev, &head);
        ASSERT_EQ(head.next, &head);

        // Test link(2)->head(1)
        int m = 2;
        ListLink<int> link(&m);
        link.insert(&head);
        ASSERT_EQ(link.next, &head);
        ASSERT_EQ(head.next, &link);
        ASSERT_EQ(link.prev, &head);
        ASSERT_EQ(head.prev, &link);

        // Test third(3)->link(2)->head(1)
        int p = 3;
        ListLink<int> third(&p);
        third.insert(&link);
        ASSERT_EQ(third.next, &link);
        ASSERT_EQ(link.next, &head);
        ASSERT_EQ(head.next, &third);
        ASSERT_EQ(third.prev, &head);
        ASSERT_EQ(head.prev, &link);
        ASSERT_EQ(link.prev, &third);

        // Check the values remain
        ASSERT_EQ(third.obj, &p);
        ASSERT_EQ(link.obj, &m);
        ASSERT_EQ(head.obj, &n);

        // Test the taker constructor
        int takerp = -p;
        ListLink<int> taker(&takerp, move(third));
        ASSERT_EQ(third.next, &third);
        ASSERT_EQ(third.prev, &third);
        ASSERT_EQ(taker.next, &link);
        ASSERT_EQ(taker.prev, &head);
        ASSERT_EQ(link.prev, &taker);
        ASSERT_EQ(head.next, &taker);

        // Check size: MUST BE SIZE - 1 because the first element is dummy
        ASSERT_EQ(third.size(), 0);
        ASSERT_EQ(link.size(), 2);
        ASSERT_EQ(taker.size(), 2);
        ASSERT_EQ(head.size(), 2);

        // Test unlink
        link.unlink();
        ASSERT_EQ(link.next, &link);
        ASSERT_EQ(link.prev, &link);
        ASSERT_EQ(taker.next, &head);
        ASSERT_EQ(head.prev, &taker);

        // Check size
        ASSERT_EQ(link.size(), 0);
        ASSERT_EQ(taker.size(), 1);
        ASSERT_EQ(head.size(), 1);

        link.insert(&head); // put it back (we know now it works)

        // Test relinking
        link.relink(&taker);
        ASSERT_EQ(link.next, &taker);
        ASSERT_EQ(taker.next, &head);
        ASSERT_EQ(head.next, &link);
        ASSERT_EQ(link.prev, &head);
        ASSERT_EQ(head.prev, &taker);
        ASSERT_EQ(taker.prev, &link);

        // Test iteration
        for(const auto &o : link)
        {
            // Iterating must skip the head directly indirect into the values, by reference
            ASSERT_TRUE(&o == &takerp || &o == &n);
        }
    }

}

