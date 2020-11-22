#include "gtest/gtest.h"
#include "acs_intr.h"
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
using ACSVM::Thread;
using ACSVM::Word;

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


TEST(ACSFunc, ACSCFChangeCeil)
{
    // TODO: move this into a fixture
    MockEnvironment environment;
    Thread *thread = environment.getFreeThread();
    GlobalScope scope(&environment, 0);
    HubScope hubScope(&scope, 0);
    MapScope mapScope(&hubScope, 0);
    MockModule module(&environment);
    Module *modulePointer = &module;
    mapScope.addModules(&modulePointer, 1);
    Script script(&module);

    Word args[1] = {};
    thread->start(&script, &mapScope, nullptr, args, 1);

    module.stringV.alloc(1, &environment.stringTable[{"FLAT1", 5}]);

    ASSERT_STREQ(thread->scopeMap->getString(0)->str, "FLAT1");

    // TODO
//    Word tag = 1;
//    Word picnum = 0;
//
//    const Word functionArgs[2] = { tag, picnum };
//
//    bool result = ACS_CF_ChangeCeil(thread, functionArgs, 2);
//    ASSERT_FALSE(result);

    // TODO: R_CheckForFlat, P_FindSectorFromTag, P_SetSectorCeilingPic

    // R_CheckForFlat TODO: R_SearchFlats, R_SearchWalls, texture_t
    // R_SearchFlats: flattable. Set up from R_InitTextureHash.
    // R_InitTextureHash: called from R_InitTextures
    // So we need a way to get some placeholder flats
    // We need wGlobalDir now
    // Calls done to the global directory: getNamespace
}
