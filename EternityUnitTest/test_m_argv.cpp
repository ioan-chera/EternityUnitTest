#include <vector>
#include "gtest/gtest.h"
#include "m_argv.h"

class MArgv : public ::testing::Test
{
protected:
	MArgv()
	{
		m_args.push_back("eternity.exe");
		m_args.push_back("-iwad");
		m_args.push_back("doom2.wad");
		m_args.push_back("-deathmatch");

		m_args.push_back("-play");
		m_args.push_back("recording");
		m_args.push_back("-playdemo");
		m_args.push_back("recording");

		m_args.push_back("-warp");
		m_args.push_back("2");
		m_args.push_back("3");
		m_args.push_back("-wart");

		m_args.push_back("5");
		m_args.push_back(nullptr);	// mandatory

		myargv = (char**)m_args.data();	// ugly cast
		myargc = (int)m_args.size() - 1;
	}
	~MArgv()
	{
		myargc = 0;
		myargv = nullptr;
	}

	std::vector<const char *> m_args;	// storage
};

TEST_F(MArgv, MCheckParm)
{
	// Environment: myargc, myargv
	ASSERT_EQ(M_CheckParm("-episode"), 0);	// inexistent parametr
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
