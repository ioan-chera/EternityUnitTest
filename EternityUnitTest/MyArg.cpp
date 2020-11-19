#include "m_argv.h"
#include "MyArg.h"

//
// Reset the globals
//
MyArg::MyArg()
{
	myargc = 0;
	myargv = nullptr;
	m_args.push_back(nullptr);
}

//
// Reset the globals
//
MyArg::~MyArg()
{
	myargc = 0;
	myargv = nullptr;
}

//
// Add stuff
//
void MyArg::add(const char *arg)
{
	m_args.pop_back();
	m_args.push_back(arg);
	m_args.push_back(nullptr);

	updateGlobals();
}

void MyArg::reset()
{
	m_args.clear();
	m_args.push_back(nullptr);
	updateGlobals();
}

//
// Update the globals
//
void MyArg::updateGlobals()
{
	myargc = (int)m_args.size() - 1;
	myargv = (char **)m_args.data();
}
