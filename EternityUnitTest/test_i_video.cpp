#include "gtest/gtest.h"
#include "doomdef.h"
#include "i_video.h"
#include "MyArg.h"

TEST(IVideo, Geom)
{
	// Sanity check of constants
	ASSERT_GE(Geom::minimumWidth, 1);
	ASSERT_GE(Geom::minimumHeight, 1);
	ASSERT_GT(MAX_SCREENWIDTH, Geom::minimumWidth);
	ASSERT_GT(MAX_SCREENHEIGHT, Geom::minimumHeight);
	ASSERT_GE(Geom::fallbackWidth, Geom::minimumWidth);
	ASSERT_GE(Geom::fallbackHeight, Geom::minimumHeight);
	ASSERT_GE(MAX_SCREENWIDTH, Geom::fallbackWidth);
	ASSERT_GE(MAX_SCREENHEIGHT, Geom::fallbackHeight);

	Geom geom;

	// Check that it starts with default values
	char strbuf[16];
	snprintf(strbuf, sizeof(strbuf), "%dx%dw", Geom::fallbackWidth, Geom::fallbackHeight);
	ASSERT_EQ(geom.toString(), strbuf);

	// Check that the common resolutions are all supported
	static const char *const commonResolutions[] =
	{
		"320x200",
		"640x480",
		"800x600",
		"1024x768",
		"1280x1024",
		"1440x900",
		"1920x1080",
		"2880x1800",
	};
	for(const char *resolution : commonResolutions)
	{
		geom.parse(resolution);
		// Also check that the w flag is added
		snprintf(strbuf, sizeof(strbuf), "%sw", resolution);
		ASSERT_EQ(geom.toString(), strbuf);
	}

	const int tooSmallWidth = Geom::minimumWidth - 1;
	const int tooSmallHeight = Geom::minimumHeight - 1;
	const int tooLargeWidth = MAX_SCREENWIDTH + 1;
	const int tooLargeHeight = MAX_SCREENHEIGHT + 1;

	// Check that invalid width but valid height produces fallback*height
	snprintf(strbuf, sizeof(strbuf), "%dx%d", tooSmallWidth, Geom::minimumHeight);
	geom.parse(strbuf);
	snprintf(strbuf, sizeof(strbuf), "%dx%dw", Geom::fallbackWidth, Geom::minimumHeight);
	ASSERT_EQ(geom.toString(), strbuf);

	// Check that valid width and invalid height produces width*fallback
	snprintf(strbuf, sizeof(strbuf), "%dx%d", Geom::minimumWidth, tooSmallHeight);
	geom.parse(strbuf);
	snprintf(strbuf, sizeof(strbuf), "%dx%dw", Geom::minimumWidth, Geom::fallbackHeight);
	ASSERT_EQ(geom.toString(), strbuf);

	// Check that + prefixes and duplicate flags get cleaned up
	geom.parse("+500x+500wddffaaavvvshn"); // allow positive signs and reduce doubles, choosing last
	ASSERT_EQ(geom.toString(), "500x500fvhn");

	// Same as above, but with opposite order.
	// NOTE: it alters the previous geom state
	geom.parse("500x500wddffdvvvahsn");
	ASSERT_EQ(geom.toString(), "500x500dan");

	// Check that capitals get reduced. Also check that old settings remain
	geom.parse("1024X768FD");
	ASSERT_EQ(geom.toString(), "1024x768dan");

	// Change just to windowed mode
	geom.parse("W");
	ASSERT_EQ(geom.toString(), "1024x768wan");

	// Remove vsync option
	geom.vsync = Geom::TriState::neutral;
	ASSERT_EQ(geom.toString(), "1024x768wn");

	// Must not parse octal values
	Geom geom2("0800x0600"); // work with a clear one
	ASSERT_EQ(geom2.toString(), "800x600w");

	// Check that xSomething means to change height
	Geom geom3("x400");
	ASSERT_EQ(geom3.toString(), "640x400w");

	// Check that a number produces a width
	geom3.parse("500");
	ASSERT_EQ(geom3.toString(), "500x400w");

	geom3.parse("x"); // make sure this junk x does nothing
	ASSERT_EQ(geom3.toString(), "500x400w");

	geom3.parse("");  // or this
	ASSERT_EQ(geom3.toString(), "500x400w");

	geom3.parse("-");  // or this
	ASSERT_EQ(geom3.toString(), "500x400w");

	// a stray zero should be interpreted as an invalid width attempt
	geom3.parse("0"); 
	snprintf(strbuf, sizeof(strbuf), "%dx400w", Geom::fallbackWidth);
	ASSERT_EQ(geom3.toString(), strbuf);

	// A missing height should not change anything
	geom3.parse("500xf");
	ASSERT_EQ(geom3.toString(), "500x400f");

	geom3.parse("-d");  // invalid character followed by flag
	ASSERT_EQ(geom3.toString(), "500x400d");

	geom3.parse("-400x-300");  // negative values should be interpreted likewise as invalid
	snprintf(strbuf, sizeof(strbuf), "%dx%dd", Geom::fallbackWidth, Geom::fallbackHeight);
	ASSERT_EQ(geom3.toString(), strbuf);

	// ridiculously large value
	snprintf(strbuf, sizeof(strbuf), "%dx%dfd", tooLargeWidth, tooLargeHeight);
	Geom geom4(strbuf);
	snprintf(strbuf, sizeof(strbuf), "%dx%dd", Geom::fallbackWidth, Geom::fallbackHeight);
	ASSERT_EQ(geom4.toString(), strbuf);

	// Limit is legal
	snprintf(strbuf, sizeof(strbuf), "%dx%d", MAX_SCREENWIDTH, MAX_SCREENHEIGHT);
	geom4.parse(strbuf);
	snprintf(strbuf, sizeof(strbuf), "%dx%dd", MAX_SCREENWIDTH, MAX_SCREENHEIGHT);
	ASSERT_EQ(geom4.toString(), strbuf);
}

//
// Arg stuff
//
using IVideoMyArg = MyArg;

TEST_F(IVideoMyArg, ICheckVideoCmdsOnce)
{
	Geom geom;	// have it defaulted
	add("eternity");
	add("-geom");
	add("800x600f");
	add("-vsync");
	add("-hardware");
	add("-noframe");
	I_CheckVideoCmdsOnce(geom);

	ASSERT_EQ(geom.toString(), "800x600fvhn");

	// Change geom to something else
	geom.width = 1000;
	ASSERT_EQ(geom.toString(), "1000x600fvhn");

	// Attempt the command again
	I_CheckVideoCmdsOnce(geom);

	// See that it can't change again
	ASSERT_EQ(geom.toString(), "1000x600fvhn");
}

TEST(IVideo, IParseResolution)
{
	// Check that native just copies the values and that it's case insensitive.
	int width, height;
	I_ParseResolution("NATIVE", width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Check that an invalid resolution will just reset to the game resolution
	width = height = 0;
	I_ParseResolution("200x200", width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Likewise check empty string
	width = height = 0;
	I_ParseResolution("", width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	char strbuf[16];

	// Check underflowing width
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", Geom::minimumWidth - 1, Geom::minimumHeight);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Check underflowing height
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", Geom::minimumWidth, Geom::minimumHeight - 1);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Check minimum size
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", Geom::minimumWidth, Geom::minimumHeight);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, Geom::minimumWidth);
	ASSERT_EQ(height, Geom::minimumHeight);

	// Check overflowing width
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", MAX_SCREENWIDTH + 1, MAX_SCREENHEIGHT);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Check overflowing height
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", MAX_SCREENWIDTH, MAX_SCREENHEIGHT + 1);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, 500);
	ASSERT_EQ(height, 400);

	// Check maximum size
	width = height = 0;
	snprintf(strbuf, sizeof(strbuf), "%dx%d", MAX_SCREENWIDTH, MAX_SCREENHEIGHT);
	I_ParseResolution(strbuf, width, height, 500, 400);
	ASSERT_EQ(width, MAX_SCREENWIDTH);
	ASSERT_EQ(height, MAX_SCREENHEIGHT);
}

TEST(IVideo, IVideoShouldLetterbox)
{
	// Environment: i_letterbox
	i_letterbox = 1;
	ASSERT_FALSE(I_VideoShouldLetterbox(1600, 900));
	ASSERT_FALSE(I_VideoShouldLetterbox(400, 300));
	ASSERT_TRUE(I_VideoShouldLetterbox(1280, 1024));
	ASSERT_TRUE(I_VideoShouldLetterbox(600, 500));

	// Check any multiple of 5 / 4 to prevent any overflows or imprecision
	for(int i = 1; i < MAX_SCREENWIDTH / 5; ++i)
		ASSERT_TRUE(I_VideoShouldLetterbox(5 * i, 4 * i));

	i_letterbox = 0;
	ASSERT_FALSE(I_VideoShouldLetterbox(1280, 1024));	// must be false now
}