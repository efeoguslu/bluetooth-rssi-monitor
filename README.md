Bluetooth Distance Measurement with RSSI

This is a C program that performs Bluetooth device scanning and connects to a chosen device based on user input. It uses the BlueZ library to interact with the Bluetooth stack.

Inputs:
This program prompts the user to choose a device to connect to or to rescan for nearby devices.

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

Outputs:
The program prints the available devices with their MAC addresses and names. If the user chooses a device, the program executes a system command to get the RSSI value of the device. It also prints the distance according to the RSSI value.
