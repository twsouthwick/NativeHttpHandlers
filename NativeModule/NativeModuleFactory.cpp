#include "pch.h"
#include "NativeModuleFactory.h"
#include "HttpContextWrapper.h"
#include <vcclr.h>
#include <string>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

typedef HRESULT(__stdcall* fRegisterModule)(DWORD, IHttpModuleRegistrationInfo*, IHttpServer*);

ref class EventCallback {
private:
	IHttpModuleFactory* _factory;
	IModuleAllocator* _allocator;
	DWORD _request;
	bool _isPost;

public:
	EventCallback(DWORD request, bool isPost, IModuleAllocator* allocator, IHttpModuleFactory* factory)
		:_request(request), _isPost(isPost), _allocator(allocator), _factory(factory) {
	}

	void Handle(System::Object^ sender, System::EventArgs^ args) {
		auto app = safe_cast<System::Web::HttpApplication^>(sender);

		CHttpModule* cmodule;
		if (_factory->GetHttpModule(&cmodule, _allocator) != S_OK) {
			// LOG IT
			return;
		}

		if (_request == RQ_BEGIN_REQUEST && !_isPost)
		{
			auto wrapper = CreateHttpContext(app->Context);
			cmodule->OnBeginRequest(wrapper->GetHttpContext(), wrapper->GetEventProvider());
		}
	}
};

class NativeModuleLoader :public INativeModuleFactory, IModuleAllocator, IHttpModuleRegistrationInfo {
private:
	const DWORD _emulatedVersion = 12;

	HINSTANCE _module;
	gcroot<System::Web::HttpApplication^> _app;
public:
	NativeModuleLoader(System::Web::HttpApplication^ app, System::String^ moduleDll) : _app(app) {
		auto strModule = msclr::interop::marshal_as<std::wstring>(moduleDll);
		_module = LoadLibrary(strModule.c_str());

		if (_module == nullptr) {
			return;
		}

		auto registerModule = (fRegisterModule)GetProcAddress(_module, "RegisterModule");

		if (registerModule == nullptr)
		{
			return;
		}

		registerModule(_emulatedVersion, this, nullptr);
	}

	bool IsLoaded() const {
		return _module != nullptr;
	}

	~NativeModuleLoader() {
		if (_module != nullptr) {
			FreeLibrary(_module);
			_module = nullptr;
		}
	}

	_Ret_opt_ _Post_writable_byte_size_(cbAllocation)
		VOID*
		AllocateMemory(
			_In_ DWORD                  cbAllocation
		) {
		return malloc(cbAllocation);
	}

	PCWSTR GetName(VOID) const {
		return L"";
	}

	HTTP_MODULE_ID GetId(VOID) const {
		return 0;
	}

	HRESULT SetRequestNotifications(
		_In_ IHttpModuleFactory* pModuleFactory,
		_In_ DWORD dwRequestNotifications,
		_In_ DWORD dwPostRequestNotifications
	) {
		if (dwRequestNotifications == RQ_BEGIN_REQUEST && dwPostRequestNotifications == 0) {
			auto callback = gcnew EventCallback(dwRequestNotifications, dwPostRequestNotifications != 0, this, pModuleFactory);
			_app->BeginRequest += gcnew System::EventHandler(callback, &EventCallback::Handle);

			return S_OK;
		}

		return E_NOTIMPL;
	}

	HRESULT SetGlobalNotifications(
		_In_ CGlobalModule* pGlobalModule,
		_In_ DWORD dwGlobalNotifications
	) {
		return E_NOTIMPL;
	}

	HRESULT SetPriorityForRequestNotification(
		_In_ DWORD                dwRequestNotification,
		_In_ PCWSTR               pszPriority
	) {
		return E_NOTIMPL;
	}

	HRESULT SetPriorityForGlobalNotification(
		_In_ DWORD                dwGlobalNotification,
		_In_ PCWSTR               pszPriority
	) {
		return E_NOTIMPL;
	}

	IModuleAllocator* GetAllocator() {
		return this;
	}

	IHttpModuleRegistrationInfo* GetRegistration() {
		return this;
	}
};

INativeModuleFactory* CreateNativeModuleFactory(System::String^ dllName, System::Web::HttpApplication^ app) {
	return new NativeModuleLoader(app, dllName);
}

