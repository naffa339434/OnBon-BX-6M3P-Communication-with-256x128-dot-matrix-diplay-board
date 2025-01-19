#include <iostream>
#include <string>
#include "../include/ICApi.h"

#ifdef HWIN64
#pragma comment(lib,"../lib64/HCatNet.lib")
#else
#pragma comment(lib,"../lib/HCatNet.lib")
#endif

static HEventCore* core = nullptr;

void ReadData(HSession* currSession, const char* data, huint32 len, void* userData) {
    std::cout << "Response: " << std::string(data, len) << std::endl;
}

void DebugLog(HSession* currSession, const char* log, huint32 len, void* userData) {
    std::cout << "Debug: " << std::string(log, len) << std::endl;
}

void NetStatus(HSession* currSession, eNetStatus status, void* userData) {
    if (status == kConnect) {
        std::cout << "Connected to device." << std::endl;
    } else if (status == kDisconnect) {
        std::cout << "Disconnected from device." << std::endl;
    }
}

void sendTextToPanel(HSession* net, const std::string& guid, const std::string& text) {
    std::string xmlMessage =
        "<?xml version='1.0' encoding='utf-8'?>"
        "<sdk guid='" + guid + "'>"
        "<in method='AddProgram'>"
        "<screen timeStamps='0'>"
        "<program guid='program-guid' type='normal'>"
        "<area guid='area-guid' alpha='255'>"
        "<rectangle x='0' y='0' width='128' height='32'/>"
        "<resources>"
        "<text guid='text-guid' singleLine='true'>"
        "<style valign='middle' align='center'/>"
        "<string>" + text + "</string>"
        "<font name='Arial' italic='false' bold='true' size='24' color='#FF0000'/>"
        "</text>"
        "</resources>"
        "</area>"
        "</program>"
        "</screen>"
        "</in>"
        "</sdk>";

    SendSDK(net, xmlMessage.c_str(), static_cast<huint32>(xmlMessage.length()));
}


void newConnectFunc(HSession* currSession, HSession* newSession, void* userData) {
    SetNetSession(newSession, kReadyReadFunc, reinterpret_cast<void*>(ReadData));
    SetNetSession(newSession, kNetStatusFunc, reinterpret_cast<void*>(NetStatus));
    SetNetSession(newSession, kDebugLogFunc, reinterpret_cast<void*>(DebugLog));
    std::cout << "New client connected." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <text>" << std::endl;
        return 1;
    }

    std::string text = argv[1];
    core = CreateEventCore();

    HSession* service = CreateNetSession(core, kSDK2Service);
    SetNetSession(service, kNewConnectFunc, reinterpret_cast<void*>(newConnectFunc));

    Run(core);

    std::string ip = "192.168.1.100";  // Replace with actual IP
    HSession* net = CreateNetSession(core, kSDK2);

    SetNetSession(net, kReadyReadFunc, reinterpret_cast<void*>(ReadData));
    SetNetSession(net, kNetStatusFunc, reinterpret_cast<void*>(NetStatus));
    SetNetSession(net, kDebugLogFunc, reinterpret_cast<void*>(DebugLog));

    if (Connect(net, ip.c_str(), 10001) == HFalse) {
        std::cerr << "Failed to connect to " << ip << std::endl;
        FreeNetSession(net);
        FreeEventCore(core);
        return 1;
    }

    std::string guid = "your-guid-here";  // Replace with actual GUID after version negotiation
    sendTextToPanel(net, guid, text);

    std::cout << "Text sent: " << text << std::endl;

    Disconnect(net);
    FreeNetSession(net);
    FreeEventCore(core);
    return 0;
}
