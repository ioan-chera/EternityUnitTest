#include <vector>
#include "gtest/gtest.h"
#include "m_argv.h"
#include "MyArg.h"

//
// Locally modified fixture
//
class MArgv : public MyArg
{
protected:
	MArgv()
	{
		add("eternity.exe");
		add("-iwad");
		add("doom2.wad");
		add("-deathmatch");

		add("-play");
		add("recording");
		add("-playdemo");
		add("recording");

		add("-warp");
		add("2");
		add("3");
		add("-wart");

		add("5");
	}
};

TEST_F(MArgv, MCheckParm)
{
	// Environment: myargc, myargv
	ASSERT_EQ(M_CheckParm("-episode"), 0);	// inexistent parameter
	ASSERT_EQ(M_CheckParm("-IWAD"), 1);	// case insensitive
	ASSERT_EQ(M_CheckParm("eternity.exe"), 0);	// program name should not really be used
	ASSERT_EQ(M_CheckParm("-deathmatch"), 3);	// exact name
	ASSERT_EQ(M_CheckParm("doom2.wad"), 2);	// unusual parameter name gets the value
}

TEST_F(MArgv, MCheckMultiParm)
{
	// Check order of priority
	const char *playdemoparms[] = { "-playdemo", "-play", nullptr };
	ASSERT_EQ(M_CheckMultiParm(playdemoparms, 1), 6);
	const char *playdemoparms2[] = { "-play", "-playdemo", nullptr };
	ASSERT_EQ(M_CheckMultiParm(playdemoparms2, 1), 4);

	// Check allowed content. Also check case insensitivity
	const char *warpparms[] = { "-Wart", "-Warp", nullptr };
	// -wart gets priority and only one is expected after it
	ASSERT_EQ(M_CheckMultiParm(warpparms, 1), 11);
	// -wart still has priority but needs two after it
	ASSERT_EQ(M_CheckMultiParm(warpparms, 2), 8);
}
