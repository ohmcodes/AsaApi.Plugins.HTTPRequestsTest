#include "API/ARK/Ark.h"
#include "Requests.h"

API::Requests& request = API::Requests::Get();

void HTTPFetchCallback(bool success, std::string results)
{
    if (success)
    {
        Log::GetLog()->info("Success");
        // Here you can parse the json response data
    }
    else
    {
        Log::GetLog()->warn("Failed {}", results);
    }
}
void HTTPFetch()
{
    std::string channel_id = "place_your_discord_id_here";
    std::string bot_token = "place_your_discord_bot_token_here";

    std::string url = FString::Format("https://discord.com/api/v10/channels/{}/messages?limit=1", channel_id).ToString();

    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "User-Agent: HTTPRequests/1.0",
        "Connection: keep-alive",
        "Accept: */*",
        "Content-Length: 0",
        "Authorization: Bot " + bot_token
    };

    bool requestSuccess = request.CreateGetRequest(url, HTTPFetchCallback, headers);

    if (!requestSuccess) {
        // Handle the case where CreatePostRequest fails
        Log::GetLog()->error("Failed to create GET request. {}", url);
    }
}

int counter = 0;
void HTTPFetchTimer()
{
    Log::GetLog()->info("Counter: {}", counter);

    HTTPFetch();

    counter++;

    // reset counter
    if (counter == 1440) counter = 0;
}


// Called when the server is ready, do post-"server ready" initialization here
void OnServerReady()
{
    Log::GetLog()->info("HTTP Requests Initialized");

    AsaApi::GetCommands().AddOnTimerCallback("HTTPFetch", &HTTPFetchTimer);
}

// Hook that triggers once when the server is ready
DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
    AShooterGameMode_BeginPlay_original(_this);

    OnServerReady();
}

// Called by AsaApi when the plugin is loaded, do pre-"server ready" initialization here
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);

    AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay,
        &AShooterGameMode_BeginPlay_original);

    if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
        OnServerReady();
}

// Called by AsaApi when the plugin is unloaded, do cleanup here
extern "C" __declspec(dllexport) void Plugin_Unload()
{
    AsaApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay);

    AsaApi::GetCommands().RemoveOnTimerCallback("HTTPFetch");
}