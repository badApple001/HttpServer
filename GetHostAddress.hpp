#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

std::string get_local_address() {
    ULONG buffer_size = 15000;
    PIP_ADAPTER_ADDRESSES adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(buffer_size);

    if (GetAdaptersAddresses(AF_INET, 0, nullptr, adapter_addresses, &buffer_size) == ERROR_BUFFER_OVERFLOW) {
        adapter_addresses = (IP_ADAPTER_ADDRESSES*)realloc(adapter_addresses, buffer_size);
    }

    std::string hostAddress = "127.0.0.1";
    if (GetAdaptersAddresses(AF_INET, 0, nullptr, adapter_addresses, &buffer_size) == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES adapter = adapter_addresses;

        while (adapter) {
            PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;

            while (address) {
                SOCKADDR_IN* sockaddr_ipv4 = (SOCKADDR_IN*)address->Address.lpSockaddr;
                char ip_address[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ip_address, INET_ADDRSTRLEN);

                // ÅÅ³ý Loopback µØÖ·
                if (strcmp(ip_address, "127.0.0.1") != 0) {
                    hostAddress = ip_address;
                }
                address = address->Next;
            }
            adapter = adapter->Next;
        }
    }

    free(adapter_addresses);
    return hostAddress;
}
