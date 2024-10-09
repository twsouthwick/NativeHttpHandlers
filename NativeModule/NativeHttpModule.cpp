#include "pch.h"
#include "NativeHttpModule.h"

void NativeHttpModule::Init(System::Web::HttpApplication^ application)
{
	_factory = CreateNativeModuleFactory(_s, application);
}

NativeHttpModule::~NativeHttpModule()
{
	if (_factory != nullptr)
	{
		delete _factory;
	}
}
