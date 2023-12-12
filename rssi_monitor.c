/*
Bluetooth Distance Measurement with RSSI

This is a C program that performs Bluetooth device scanning and connects to a chosen device based on user input. It uses the BlueZ library to interact with the Bluetooth stack.

Inputs: This program prompts the user to choose a device to connect to or to rescan for nearby devices.

Flow:

1. The program starts by initializing variables and allocating memory for the "inquiry_info" array to store information about discovered devices.
2. It performs a Bluetooth device inquiry using the "hci_inquiry" function, which retrieves information about nearby devices.
3. The program then prints the available devices with their MAC addresses and names.
4. The user is prompted to choose a device to connect to or to rescan for devices.
5. If the user chooses to rescan, the allocated memory is freed and the program continues to the next iteration of the loop.
6. If the user chooses a device, the target address is set based on the user's choice.
7. Within an inner loop, the program performs another Bluetooth device inquiry to check if the target device is still in range.
8. If the target device is found, the program executes a system command to get the RSSI (Received Signal Strength Indication) value of the device.
9. The RSSI is then converted into distance using the "log-distance path loss model" method and printed to the output console.

Outputs: The program prints the available devices with their MAC addresses and names. If the user chooses a device, the program executes a system command to get the RSSI value of the device. 
It also prints the distance according to the RSSI value.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <math.h>

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

double rssiToDistance(int rssi) {
    double n = 2;
    double mp = -69;
    return round(pow(10, ((mp - (double)rssi) / (10 * n))) * 100) / 100;
}

int main() {
    inquiry_info *devices = NULL;  // Change the type to pointer
    char target_addr[19];    // Address of the device to connect

    while (1) {
        printf("Scanning for nearby Bluetooth devices...\n");

        // Allocate memory for the inquiry_info array
        devices = (inquiry_info *)calloc(MAX_DEVICES, sizeof(inquiry_info));
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
            char command[256];
            sprintf(command, "sudo btmgmt find | grep \"%s\" | awk '{print $7}'", target_addr);
            // system(command);

            FILE *fp;
            char output[256];

            fp = popen(command, "r");

            if (fp == NULL) {
                perror("Failed to run command");
                exit(1);
            }

            while (fgets(output, sizeof(output) - 1, fp) != NULL) {
                int rssi = atoi(output);
                printf("Distance: %.2lf meters at %d RSSI\n", rssiToDistance(rssi), rssi);
            }

            pclose(fp);
        }
    }

    // Clean up
    free(devices);  // Free the allocated memory
    return 0;
}

