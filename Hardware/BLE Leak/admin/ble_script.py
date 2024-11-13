import pygatt
import time
import binascii
import csv
import datetime
from binascii import hexlify
import logging
from pygatt.backends import GATTToolBackend


class BLEHandler:
    def __init__(self, device_address, notify_uuid, write_uuid, output_file="ble_data.csv"):
        self.device_address = device_address
        self.notify_uuid = notify_uuid
        self.write_uuid = write_uuid
        self.output_file = output_file
        self.adapter = None
        self.device = None

        # Initialize CSV file with headers
        with open(self.output_file, 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=['hex_data'])
            writer.writeheader()

    def handle_notification(self, handle, value):
        """Callback function to handle notifications"""
        # timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")
        hex_data = hexlify(value).decode('utf-8')
        print(f"Notification received: {hex_data}")

        # Save to CSV file
        with open(self.output_file, 'a', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=['hex_data'])
            writer.writerow({
                'hex_data': hex_data
            })

    def connect_and_run(self):
        """Main function to connect and handle BLE operations"""
        try:
            # Initialize the adapter
            self.adapter = GATTToolBackend()
            self.adapter.start()

            print(f"Connecting to device: {self.device_address}")
            self.device = self.adapter.connect(self.device_address)
            print("Connected!")

            # Subscribe to notifications
            self.device.subscribe(self.notify_uuid, callback=self.handle_notification)
            print(f"Subscribed to notifications on {self.notify_uuid}")

            # Main loop for writing data
            while True:
                try:
                    # Example data to write - modify as needed
                    write_data = bytearray([0x01, 0x02, 0x03])

                    # Write to the characteristic
                    self.device.char_write(self.write_uuid, write_data)
                    print(f"Wrote data: {hexlify(write_data).decode('utf-8')}")

                    # Wait before next write
                    time.sleep(1)  # Adjust timing as needed

                except pygatt.exceptions.NotificationTimeout:
                    print("Notification timeout, continuing...")
                except Exception as e:
                    print(f"Error during operation: {e}")
                    break

        except Exception as e:
            print(f"Error: {e}")
        finally:
            if self.device:
                try:
                    self.device.disconnect()
                except:
                    pass
            if self.adapter:
                try:
                    self.adapter.stop()
                except:
                    pass

def main():
    # Enable debug logging if needed
    # logging.basicConfig(level=logging.DEBUG)

    # Replace these with your device's address and characteristic UUIDs
    # DEVICE_ADDRESS = ""  # MAC address of your BLE device
    DEVICE_ADDRESS = "30:30:f9:27:7d:7a"  # MAC address of your BLE device
    NOTIFY_UUID = "C23E5743-B8DB-424F-8102-8942A01CEEB2"  # UUID of notification characteristic
    WRITE_UUID  = "A81E5743-B8DB-424F-8102-8942A01CEEB2"  # UUID of write characteristic

    handler = BLEHandler(DEVICE_ADDRESS, NOTIFY_UUID, WRITE_UUID)

    try:
        handler.connect_and_run()
    except KeyboardInterrupt:
        print("\nStopping application...")
    except Exception as e:
        print(f"Application error: {e}")

if __name__ == "__main__":
    main()
