#pragma once
#include <memory>

class INativeModuleFactory
{
public:
	virtual IModuleAllocator* GetAllocator() = 0;
	virtual IHttpModuleRegistrationInfo* GetRegistration() = 0;
};

INativeModuleFactory* CreateNativeModuleFactory(System::String^ dllName, System::Web::HttpApplication^ app);

