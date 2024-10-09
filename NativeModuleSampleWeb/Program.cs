using Microsoft.AspNetCore.Http.Features;
using Microsoft.AspNetCore.SystemWebAdapters;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddSystemWebAdapters()
    .AddHttpApplication(options =>
    {
        options.RegisterModule<SampleNativeModule>();
    });

var app = builder.Build();

app.Use((ctx, next) =>
{
    // If running on Kestrel, server variables aren't available
    if (ctx.Features.Get<IServerVariablesFeature>() is null)
    {
        ctx.Features.Set<IServerVariablesFeature>(new ServerVariablesFeature());
    }

    return next(ctx);
});

app.UseSystemWebAdapters();

app.MapGet("/", (HttpContext context) =>
{
    return context.AsSystemWeb().Request.ServerVariables["FromNativeModule"];
});

app.Run();

sealed class SampleNativeModule() : NativeHttpModule(Path.Combine(AppContext.BaseDirectory, "SampleModule.dll"))
{
}

sealed class ServerVariablesFeature : IServerVariablesFeature
{
    private readonly Dictionary<string, string> _variables = [];

    public string? this[string variableName]
    {
        get => _variables.TryGetValue(variableName, out var existing) ? existing : null;
        set
        {
            if (value is null)
            {
                _variables.Remove(variableName);
            }
            else
            {
                _variables[variableName] = value;
            }
        }
    }
}
