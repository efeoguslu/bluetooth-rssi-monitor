
import bluetooth
import time


def display_nearby_devices():
    print("Scanning for nearby devices...")
    
    # Scan for nearby devices
    nearby_devices = bluetooth.discover_devices(duration=8, lookup_names=True)

    if not nearby_devices:
        print("No nearby devices found.")
    else:
        print("Available devices:")
        for i, (addr, name) in enumerate(nearby_devices):
            print(f"{i + 1}. {name} ({addr})")

    return nearby_devices  # Return the list of nearby devices

def get_user_choice(nearby_devices):
    while True:
        try:
            choice = int(input("Enter the number of the device to monitor (0 to rescan): "))
            if 0 <= choice <= len(nearby_devices):
                return choice
            else:
                print("Invalid choice. Please enter a valid number.")
        except ValueError:
            print("Invalid input. Please enter a number.")

def get_rssi(device_address, duration=5):
    print(f"Scanning for {device_address}...")

    # Search for nearby devices
    nearby_devices = bluetooth.discover_devices(duration=duration, lookup_names=True)

    for addr, name in nearby_devices:
        if addr == device_address:
            print(f"Found {name} with address {addr}")
            
            # Get information about the device
            services = bluetooth.find_service(address=device_address)
            rssi = services[0]['rssi']
            
            print(f"RSSI: {rssi} dBm")

            return rssi

    print(f"Device {device_address} not found.")
    return None

if __name__ == "__main__":
    while True:
        nearby_devices = display_nearby_devices()
        
        # Get user choice
        choice = get_user_choice(nearby_devices)

        if choice == 0:
            continue  # Rescan devices

        selected_device = nearby_devices[choice - 1]
        android_device_address = selected_device[0]
        print(f"Monitoring RSSI for {selected_device[1]} ({android_device_address})")

        while True:
            rssi_value = get_rssi(android_device_address)

            if rssi_value is not None:
                # You can add logic here based on the RSSI value to determine distance
                # For example, if rssi_value > -70, the device is close; if rssi_value <= -70, it's far.

                # Sleep for a while before scanning again
                time.sleep(5)
            else:
                break 