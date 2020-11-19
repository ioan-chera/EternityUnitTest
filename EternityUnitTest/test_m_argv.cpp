#include "gtest/gtest.h"
#include "m_argv.h"

class MArgv : public ::testing::Test
{
protected:
	MArgv()
	{

		myargc = 4;
		myargv = new char *[5];
		myargv[0] = (char *)"eternity.exe";
		myargv[1] = (char *)"-iwad";
		myargv[2] = (char *)"doom2.wad";
		myargv[3] = (char *)"-deathmatch";
		myargv[4] = nullptr;
	}
	~MArgv()
	{
		myargc = 0;
		delete[] myargv;
		myargv = nullptr;
	}
};

TEST_F(MArgv, MCheckParm)
{
	// Environment: myargc, myargv
	ASSERT_EQ(M_CheckParm("-warp"), 0);
	ASSERT_EQ(M_CheckParm("-IWAD"), 1);	// case insensitive
	ASSERT_EQ(M_CheckParm("eternity.exe"), 0);
	ASSERT_EQ(M_CheckParm("-deathmatch"), 3);	// exact name
	ASSERT_EQ(M_CheckParm("doom2.wad"), 2);	// unusual parameter name gets the value
}

TEST(MArgv, MCheckMultiParm)
{
	// Environment: myargc
	// TODO
}
