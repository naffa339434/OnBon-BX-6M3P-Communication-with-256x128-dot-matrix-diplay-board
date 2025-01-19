#include "../include/ICApi.h"
#include <iostream>
#include <string>

// Create event core (needed for all operations)
HEventCore* core = CreateEventCore();

// Create network session for SDK communication
HSession* net = CreateNetSession(core, kSDK2);

static void ReadData(HSession *currSession, const char *data, huint32 len, void *userData) {
    printf("Received: %s\n", data);
}

static void NetStatus(HSession *currSession, eNetStatus status, void *userData) {
    if (status == kConnect) {
        printf("Connected\n");
    } else {
        printf("Disconnected\n");
    }
}

// Register callbacks
SetNetSession(net, kReadyReadFunc, reinterpret_cast<void *>(ReadData));
SetNetSession(net, kNetStatusFunc, reinterpret_cast<void *>(NetStatus));

// Connect to the LED panel using its IP
if (Connect(net, "192.168.1.100", 10001) == HFalse) {
    std::cout << "Connection failed" << std::endl;
    return;
}
// Example command to get device info
const char* command = 
    "<?xml version='1.0' encoding='utf-8'?>\n"
    "<sdk guid=\"YOUR_GUID\">\n"
    "    <in method=\"GetDeviceInfo\"/>\n"
    "</sdk>";

SendSDK(net, command, strlen(command));


// Send a file (type 0 for images)
SendFile(net, "path/to/your/image.jpg", 0);

// Disconnect and cleanup
Disconnect(net);
FreeNetSession(net);
FreeEventCore(core);