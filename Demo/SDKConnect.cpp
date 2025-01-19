#include <iostream>
#include <string.h>

#include "../include/ICApi.h"

#ifdef HWIN64
#pragma comment(lib,"../lib64/HCatNet.lib")
#else
#pragma comment(lib,"../lib/HCatNet.lib")
#endif

#define GETDEVICEINFO_TEXT          "<?xml version=\'1.0\' encoding=\'utf-8\'?>\n" \
                                    "<sdk guid=\"##GUID\">\n"                      \
                                    "    <in method=\"GetDeviceInfo\"/>\n"         \
                                    "</sdk>"

static HEventCore *core;

static void ReadData(HSession *currSession, const char *data, huint32 len, void *userData) {
    printf("%s\n", data);
}

static void DebugLog(HSession *currSession, const char *log, huint32 len, void *userData) {
    printf("%s\n", log);
    fflush(stdout);
}

static void NetStatus(HSession *currSession, eNetStatus status, void *userData) {
    switch (status) {
    case kConnect:
        printf("connect\n");
        break;
    case kDisconnect:
        printf("disconnect\n");
        break;
    default:
        break;
    }
    fflush(stdout);
}

int main() {
    // 创建一个单例的事件循环
    core = CreateEventCore();

    // 创建SDK网络会话
    HSession *net = CreateNetSession(core, kSDK2);

    // 设置网络会话的相关回调
    SetNetSession(net, kReadyReadFunc, reinterpret_cast<void *>(ReadData));
    SetNetSession(net, kReadyReadUserData, net);
    SetNetSession(net, kNetStatusFunc, reinterpret_cast<void *>(NetStatus));
    SetNetSession(net, kNetStatusUserData, net);
    SetNetSession(net, kDebugLogFunc, reinterpret_cast<void *>(DebugLog));

    // 连接到设备
    if (Connect(net, "127.0.0.1", 32000)) { // Replace with actual IP and port
    // 发送 "HELLO" 消息
    const char *message = "HELLO";
    SendSDK(net, message, strlen(message));
} else {
    printf("Failed to connect\n");
}

    // 运行事件循环
    Run(core);

    // 清理
    Disconnect(net);
    FreeNetSession(net);
    FreeEventCore(core);

    return 0;
}