# SystemWebAdaptersNativeModule

A sample of using C++/CLI to load a native IIS module for use in ASP.NET CORE using System.Web adapters. This is by no means complete (it will throw for most operations), but highlights how it may be done.

- *NativeModule* The C++/CLI project that exposes a managed `NativeModule` that can be derived for a native module
- *SampleModule* A basic IIS native module that just sets a server variable
- *NativeModuleSampleWeb* A simple ASP.NET Core site that uses the native module to hook into the emulated IIS pipeline

## Simple setup

```csharp
using Microsoft.AspNetCore.Http.Features;
using Microsoft.AspNetCore.SystemWebAdapters;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddSystemWebAdapters()
    .AddHttpApplication(options =>
    {
        options.RegisterModule<SampleNativeModule>();
    });

var app = builder.Build();

app.UseSystemWebAdapters();

// .. Map endpoints

app.Run();

sealed class SampleNativeModule() : NativeHttpModule(Path.Combine(AppContext.BaseDirectory, "SampleModule.dll"))
{
}
```
