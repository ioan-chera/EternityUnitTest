#include "gtest/gtest.h"
#include "acs_intr.h"
#include "ACSVM/Module.hpp"
#include "ACSVM/Scope.hpp"

#if 0
class MockModuleName : public ACSVM::ModuleName
{
public:
    MockModuleName() : ACSVM::ModuleName(nullptr, nullptr, 0)
    {
    }
};

class MockModule : public ACSVM::Module
{
public:
    MockModule(ACSVM::Environment *env) : ACSVM::Module(env, MockModuleName())
    {
        static const char *strdata = "jackson";
        stringV.alloc(1, new ACSVM::String(ACSVM::StringData(strdata, strdata + sizeof(strdata) - 1), 0));


    }
};

class MockEnvironment : public ACSVM::Environment
{
protected:
    void loadModule(ACSVM::Module *module) override
    {
    }
};

class MockString : public ACSVM::String
{

};

class MockMapScope : public ACSVM::MapScope
{
public:
    MockMapScope(ACSVM::HubScope *hub, ACSVM::Word id) : ACSVM::MapScope(hub, id)
    {
        module0 = new MockModule(hub->env);
    }

};

class MockThread : public ACSVM::Thread
{
public:
    MockThread() : ACSVM::Thread(nullptr)
    {
        mEnvironment = new MockEnvironment;
        mGlobalScope = new ACSVM::GlobalScope(mEnvironment, 0);
        mHubScope = new ACSVM::HubScope(mGlobalScope, 0);
        scopeMap = new MockMapScope(mHubScope, 0);
    }

    virtual ~MockThread()
    {
        delete scopeMap;
        delete mHubScope;
        delete mGlobalScope;
        delete mEnvironment;
    }

    MockEnvironment *mEnvironment;
    ACSVM::GlobalScope *mGlobalScope;
    ACSVM::HubScope *mHubScope;
};


TEST(ACSFunc, ACSCFChangeCeil)
{
    
}
#endif
