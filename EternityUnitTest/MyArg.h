#pragma once
#include <vector>
#include "gtest/gtest.h"

//
// Fixture for myargc, myargv
//
class MyArg : public ::testing::Test
{
protected:
	MyArg();
	virtual ~MyArg();

	void add(const char *arg);
	void reset();
private:
	void updateGlobals();

	std::vector<const char *> m_args;
};
