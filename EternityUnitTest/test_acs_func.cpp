#include "gtest/gtest.h"
#include "acs_intr.h"
#include "d_files.h"
#include "d_gi.h"
#include "p_spec.h"
#include "r_data.h"
#include "r_state.h"
#include "w_wad.h"
#include "ACSVM/Module.hpp"
#include "ACSVM/Scope.hpp"
#include "ACSVM/Script.hpp"

using ACSVM::Environment;
using ACSVM::GlobalScope;
using ACSVM::HubScope;
using ACSVM::MapScope;
using ACSVM::Module;
using ACSVM::ModuleName;
using ACSVM::Script;
using ACSVM::StringData;
using ACSVM::Thread;
using ACSVM::Word;
using std::vector;

void R_InitTextures();

class MockEnvironment : public Environment
{
protected:
    void loadModule(Module *module) override
    {
    }
};

class MockModule : public Module
{
public:
    MockModule(Environment *env) : Module(env, ModuleName(nullptr, nullptr, 0))
    {
    }
};

class ACSFuncThreadFixture : public ::testing::Test
{
protected:
    ACSFuncThreadFixture() :
        mThread(mEnvironment.getFreeThread()),
        mGlobalScope(&mEnvironment, 0),
        mHubScope(&mGlobalScope, 0),
        mMapScope(&mHubScope, 0),
        mModule(&mEnvironment),
        mModulePointer(&mModule),
        mScript(&mModule)
    {
        mMapScope.addModules(&mModulePointer, 1);
        Word args[1] = {};
        mThread->start(&mScript, &mMapScope, nullptr, args, 1);
    }

    void addString(const char *string)
    {
        StringData data(string, strlen(string));
        size_t size = mModule.stringV.size();
        mModule.stringV.realloc(size + 1, &mEnvironment.stringTable[data]);
        ASSERT_STREQ(mThread->scopeMap->getString(static_cast<Word>(size))->str, string);
    }

    MockEnvironment mEnvironment;
    Thread *mThread;
    GlobalScope mGlobalScope;
    HubScope mHubScope;
    MapScope mMapScope;
    MockModule mModule;
    Module *mModulePointer;
    Script mScript;
};

static void addTextureWad()
{
    D_SetGameModeInfo(retail, doom);
    D_AddFile("textures.wad", lumpinfo_t::ns_global, nullptr, 0, DAF_NONE);
    wGlobalDir.initMultipleFiles(wadfiles);
    R_InitTextures();
}

static sector_t makeEmptySector()
{
    sector_t sector = {};
    sector.firsttag = sector.nexttag = -1;
    return sector;
}

static void tagSector(vector<sector_t> &sectors, int index, int tag)
{
    int size = static_cast<int>(sectors.size());
    int hash = tag % size;
    if(sectors[hash].firsttag == -1)
    {
        sectors[hash].firsttag = index;
        sectors[index].tag = tag;
    }
    else
    {
        bool found = false;
        int *lastNext = nullptr;
        for(int current = sectors[hash].firsttag; current != -1; current = sectors[current].nexttag)
        {
            lastNext = &sectors[current].nexttag;
            if(current == index)
            {
                sectors[current].tag = tag;
                found = true;
                break;
            }
        }
        if(!found)
        {
            *lastNext = index;
            sectors[index].tag = tag;
        }
    }
}

TEST_F(ACSFuncThreadFixture, ACSCFChangeCeilAndFloor)
{
    addString("FLAT");
    addString("F_SKY1");
    addString("NOFLAT");

    addTextureWad();

    int flatIndex = R_CheckForFlat("FLAT");
    ASSERT_NE(flatIndex, -1);
    ASSERT_EQ(R_CheckForFlat("NOFLAT"), -1);

    vector<sector_t> sectors = { makeEmptySector(), makeEmptySector(), makeEmptySector() };
    ::sectors = sectors.data();
    ::numsectors = static_cast<int>(sectors.size());
    tagSector(sectors, 0, 0);
    tagSector(sectors, 1, 1);
    tagSector(sectors, 2, 2);

    ASSERT_EQ(P_FindSectorFromTag(1, -1), 1);
    ASSERT_EQ(P_FindSectorFromTag(0, -1), 0);
    ASSERT_EQ(P_FindSectorFromTag(2, -1), 2);

    Word functionArgs[2] = { 1, 0 };    // FLAT (have it)

    // Expect setting known flat results in success
    ASSERT_NE(sectors[1].srf.ceiling.pic, flatIndex);
    bool result = ACS_CF_ChangeCeil(mThread, functionArgs, 2);
    ASSERT_FALSE(result);
    ASSERT_EQ(sectors[1].srf.ceiling.pic, flatIndex);

    // Same for floor
    ASSERT_NE(sectors[1].srf.floor.pic, flatIndex);
    result = ACS_CF_ChangeFloor(mThread, functionArgs, 2);
    ASSERT_FALSE(result);
    ASSERT_EQ(sectors[1].srf.floor.pic, flatIndex);

    functionArgs[0] = 2;
    functionArgs[1] = 2;    // NOFLAT (don't have it)

    // Expect that it won't change if flat is absent
    int pic = sectors[2].srf.ceiling.pic;
    result = ACS_CF_ChangeCeil(mThread, functionArgs, 2);
    ASSERT_FALSE(result);
    ASSERT_EQ(sectors[2].srf.ceiling.pic, pic);

    // But floor WILL work, falling back to the invalid texture
    result = ACS_CF_ChangeFloor(mThread, functionArgs, 2);
    ASSERT_FALSE(result);
    ASSERT_EQ(sectors[2].srf.floor.pic, texturecount - 1);
}
