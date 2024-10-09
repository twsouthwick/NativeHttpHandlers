#pragma once

#include "NativeModuleFactory.h"
#include <memory>

public ref class NativeHttpModule : public System::Web::IHttpModule
{
private:
	System::String^ _s;

	// I'd prefer to use a shared_ptr, but the c++/cli system makes that tricky in managed classes
	INativeModuleFactory* _factory;

public:
	NativeHttpModule(System::String^ s) :_s(s) {
	}

	virtual void Init(System::Web::HttpApplication^ application);

	~NativeHttpModule();
};

