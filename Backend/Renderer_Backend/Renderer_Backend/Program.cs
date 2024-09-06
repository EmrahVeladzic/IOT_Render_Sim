using Microsoft.AspNetCore.Authentication.Negotiate;
using Microsoft.AspNetCore.Cors.Infrastructure;
using Microsoft.Extensions.Hosting.Internal;
using Renderer_Backend.Serial;
using System.IO.Ports;



var builder = WebApplication.CreateBuilder(args);

// Add services to the container.

builder.Services.AddCors(options => {

    options.AddPolicy("ALL", policy => {

        policy.AllowAnyOrigin().AllowAnyHeader().AllowAnyMethod();
    
    });

});


builder.Services.AddControllers();
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddAuthentication(NegotiateDefaults.AuthenticationScheme)
.AddNegotiate();





builder.Services.AddAuthorization(options =>
{
    // By default, all incoming requests will be authorized according to the default policy.
    options.FallbackPolicy = options.DefaultPolicy;
});

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.Lifetime.ApplicationStarted.Register(() =>
{
    Task.Run(() => Renderer.Update());
});

app.Lifetime.ApplicationStopping.Register(() =>
{
    Renderer.V_Port!.Close();
});


app.UseHttpsRedirection();

app.UseCors("ALL");

app.UseAuthorization();

app.MapControllers();

app.Run();


