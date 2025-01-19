#include <iostream>
#include <string>
#include <thread>

#include "../include/ICApi.h"

#ifdef HWIN64
#pragma comment(lib,"../lib/HCatNet.lib")  // Link appropriate library for 64-bit or 32-bit build
#else
#pragma comment(lib,"../lib/HCatNet.lib")
#endif

static HEventCore *core = nullptr;  // Pointer to event core for managing event loop

// Callback function to handle detected device information
static void DeviceInfo(HSession *currSession, const char *id, huint32 idLen, const char *ip, huint32 ipLen, const char *readData, huint32 dataLen, void *userData) {
    printf("id[%s], ip[%s]\n", id, ip);  // Print device ID and IP address
}

int main()
{
    // Create a singleton event loop
    core = CreateEventCore();

    // Create a session for detecting device IDs and IPs
    HSession *findDevice = CreateNetSession(core, kDetectDevice);
    SetNetSession(findDevice, kDeviceInfoFunc, DeviceInfo);  // Set callback for device information

    // In detection sessions, only a connection attempt is needed; parameters are not used
    Connect(findDevice, "", 0);

    // Each call to SendSDK triggers sending detection data
    SendSDK(findDevice, "", 0);

    Run(core);  // Start the event loop to continuously handle events

    // Periodically resend detection data every 3 seconds
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        SendSDK(findDevice, "", 0);
    }

    FreeNetSession(findDevice);  // Release the session resources
    FreeEventCore(core);  // Release the event core resources
    return 0;
}
