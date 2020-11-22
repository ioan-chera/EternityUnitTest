#include "gtest/gtest.h"
#include "ACSVM/Vector.hpp"

using ACSVM::Vector;

class ObjClass
{
public:
    ObjClass()
    {
        data[0] = data[1] = data[2] = 6;
    }
    ObjClass(int a, int b, int c)
    {
        data[0] = a;
        data[1] = b;
        data[2] = c;
    }
    int data[3];
};

TEST(ACSVMVector, Vector)
{
    {
        Vector<int> vector;
        ASSERT_EQ(vector.size(), 0);
    }
    {
        Vector<int> vector(3);
        ASSERT_EQ(vector.size(), 3);
        for(int i = 0; i < 3; ++i)
            ASSERT_EQ(vector[i], 0);
        vector[1] = 23;
        ASSERT_EQ(vector[0], 0);
        ASSERT_EQ(vector[1], 23);
        ASSERT_EQ(vector[2], 0);
    }
    {
        Vector<ObjClass> vector(3);
        for(int i = 0; i < 3; ++i)
        {
            ASSERT_EQ(vector[i].data[0], 6);
            ASSERT_EQ(vector[i].data[1], 6);
            ASSERT_EQ(vector[i].data[2], 6);
        }
        vector.free();
        ASSERT_EQ(vector.size(), 0);
        vector.alloc(1, 4, 5, 6);
        ASSERT_EQ(vector.size(), 1);
        ASSERT_EQ(vector[0].data[0], 4);
        ASSERT_EQ(vector[0].data[1], 5);
        ASSERT_EQ(vector[0].data[2], 6);
        vector.alloc(2, 5, 6, 7);
        ASSERT_EQ(vector.size(), 2);
        for(int i = 0; i < 2; ++i)
        {
            ASSERT_EQ(vector[i].data[0], 5);
            ASSERT_EQ(vector[i].data[1], 6);
            ASSERT_EQ(vector[i].data[2], 7);
        }
        vector.realloc(3, 6, 7, 8);
        ASSERT_EQ(vector.size(), 3);
        for(int i = 0; i < 2; ++i)
        {
            ASSERT_EQ(vector[i].data[0], 5);
            ASSERT_EQ(vector[i].data[1], 6);
            ASSERT_EQ(vector[i].data[2], 7);
        }
        ASSERT_EQ(vector[2].data[0], 6);
        ASSERT_EQ(vector[2].data[1], 7);
        ASSERT_EQ(vector[2].data[2], 8);

        Vector<ObjClass> otherVector(1);
        vector.swap(otherVector);
        ASSERT_EQ(vector.size(), 1);
        ASSERT_EQ(vector[0].data[0], 6);
        ASSERT_EQ(vector[0].data[1], 6);
        ASSERT_EQ(vector[0].data[2], 6);
        ASSERT_EQ(otherVector.size(), 3);
        for(int i = 0; i < 2; ++i)
        {
            ASSERT_EQ(otherVector[i].data[0], 5);
            ASSERT_EQ(otherVector[i].data[1], 6);
            ASSERT_EQ(otherVector[i].data[2], 7);
        }
        ASSERT_EQ(otherVector[2].data[0], 6);
        ASSERT_EQ(otherVector[2].data[1], 7);
        ASSERT_EQ(otherVector[2].data[2], 8);

        Vector<ObjClass> takerVector(std::move(vector));
        ASSERT_EQ(vector.size(), 0);
        ASSERT_EQ(takerVector.size(), 1);
        ASSERT_EQ(takerVector[0].data[0], 6);
        ASSERT_EQ(takerVector[0].data[1], 6);
        ASSERT_EQ(takerVector[0].data[2], 6);

        Vector<ObjClass>::iterator it = otherVector.begin();
        ASSERT_EQ(it, &otherVector[0]);
        ASSERT_EQ(otherVector.end() - 1, &otherVector[2]);
    }
}
