
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define MAX_DEVICES 10

int find_device(const char* target_addr, inquiry_info* devices, int num_devices) {
    for (int i = 0; i < num_devices; i++) {
        char addr[19] = { 0 };
        ba2str(&(devices[i].bdaddr), addr);
        if (strcmp(addr, target_addr) == 0) {
            return i;
        }
    }
    return -1;  // Device not found
}

int main() {
    inquiry_info *devices = NULL;  // Change the type to pointer
    char target_addr[19];    // Address of the device to connect

    while (1) {
        printf("Scanning for nearby Bluetooth devices...\n");

        // Allocate memory for the inquiry_info array
        devices = (inquiry_info *)malloc(MAX_DEVICES * sizeof(inquiry_info));
        if (devices == NULL) {
            perror("Failed to allocate memory");
            exit(1);
        }

        // Perform Bluetooth device inquiry
        int num_devices = hci_inquiry(hci_get_route(NULL), 8, MAX_DEVICES, NULL, &devices, IREQ_CACHE_FLUSH);

        if (num_devices < 0) {
            perror("Inquiry failed");
            free(devices);  // Free the allocated memory before exiting
            exit(1);
        }

        // Print available devices with MAC addresses and names
        printf("Available devices:\n");
        for (int i = 0; i < num_devices; i++) {
            char addr[19] = { 0 };
            char name[248] = { 0 };
            ba2str(&(devices[i].bdaddr), addr);

            // Try to read the remote name
            if (hci_read_remote_name(hci_open_dev(hci_get_route(NULL)), &(devices[i].bdaddr), sizeof(name), name, 0) < 0) {
                strcpy(name, "[unknown]");
            }

            printf("%d: %s (%s)\n", i + 1, addr, name);
        }

        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        // Ask the user to choose a device or rescan
        int choice;
        printf("Enter the number of the device to connect or enter 0 to rescan: ");
        scanf("%d", &choice);

        if (choice == 0) {
            // Free the allocated memory before rescanning
            free(devices);

            // Continue to the next iteration of the loop to rescan
            continue;
        } else if (choice < 1 || choice > num_devices) {
            fprintf(stderr, "Invalid choice. Exiting.\n");
            free(devices);  // Free the allocated memory before exiting
            exit(1);
        } else {
            // Set the target address to the chosen device
            ba2str(&(devices[choice - 1].bdaddr), target_addr);
        }

        while (1) {
            // Perform Bluetooth device inquiry within the loop
            num_devices = hci_inquiry(hci_get_route(NULL), 8, MAX_DEVICES, NULL, &devices, IREQ_CACHE_FLUSH);

            if (num_devices < 0) {
                perror("Inquiry failed");
                free(devices);  // Free the allocated memory before exiting
                exit(1);
            }

            // Find the index of the target device
            int target_index = find_device(target_addr, devices, num_devices);

            if (target_index == -1) {
                fprintf(stderr, "Device not found. Exiting.\n");
                break;
            }

            // Get RSSI
            // inquiry_info_with_rssi *ir = (inquiry_info_with_rssi*)(&(devices[target_index]));
            // printf("RSSI: %d dBm\n", ir->rssi);

            // Delay between RSSI readings

            char command[256];
            sprintf(command, "sudo btmgmt find | grep \"%s\" | awk '{print $7}'", target_addr);
            system(command);

            // sleep(1);
        }
    }

    // Clean up
    free(devices);  // Free the allocated memory
    return 0;
}

