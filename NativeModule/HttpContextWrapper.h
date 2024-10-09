#pragma once
#include <memory>

class IHttpContextWrapper 
{
public:
	virtual IHttpContext* GetHttpContext() = 0;
	virtual IHttpEventProvider* GetEventProvider() = 0;
};

std::shared_ptr<IHttpContextWrapper> CreateHttpContext(System::Web::HttpContext^ context);

