# ESP32 LED Matrix Scrolling Display

A WiFi-enabled scrolling text display using ESP32 and HT1632 LED matrix panels. This project creates a web-controlled LED matrix that displays scrolling text messages updated via REST API.

## Hardware Requirements

- ESP32 development board (NodeMCU-32S)
- HT1632-based LED matrix panel (32x8 or similar)
- Jumper wires for connections

## Pin Configuration

| ESP32 Pin | LED Matrix | Description |
|-----------|------------|-------------|
| GPIO 16   | CS1        | Chip Select |
| GPIO 18   | WR         | Write Clock |
| GPIO 17   | DATA       | Data Line   |

## Features

- **WiFi Connectivity**: Connects to your local WiFi network
- **Scrolling Text Display**: Smooth horizontal text scrolling
- **REST API Control**: Update display text remotely via HTTP POST
- **Task Scheduler**: Non-blocking operation using cooperative multitasking
- **Debug Output**: Comprehensive serial logging for troubleshooting
- **Auto-reconnect**: Automatic WiFi reconnection on connection loss

## Software Dependencies

This project uses PlatformIO and includes the following libraries:

- **HT1632 Library**: For controlling the LED matrix display
- **TaskScheduler**: For non-blocking task management
- **ArduinoJson**: For JSON parsing in REST API
- **WebServer**: Built-in ESP32 web server functionality

## Building and Installation

### Prerequisites

1. Install [PlatformIO](https://platformio.org/) IDE or VS Code with PlatformIO extension
2. Clone or download this repository

### Configuration

1. **WiFi Setup**: Edit `include/config.h` to set your WiFi credentials:
   ```cpp
   const char* ssid = "YourWiFiSSID";
   const char* password = "YourWiFiPassword";
   ```

2. **Hardware Connections**: Wire your LED matrix according to the pin configuration above

### Build and Upload

1. Open the project in PlatformIO
2. Build the project:
   ```bash
   pio run
   ```
3. Upload to your ESP32:
   ```bash
   pio run --target upload
   ```
4. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Usage

### Initial Setup

1. Power on your ESP32 with the LED matrix connected
2. Open the serial monitor (115200 baud) to view debug output
3. Wait for WiFi connection - the device will display its IP address
4. The web server will start automatically on port 80

### Default Display

The device starts with the message "Hello, how are you?" scrolling across the display.

## REST API Documentation

### Update Scrolling Text

Update the text displayed on the LED matrix.

**Endpoint:** `/api/scroll`  
**Method:** `POST`  
**Content-Type:** `application/json`

#### Request Body

```json
{
  "text": "Your message here"
}
```

#### Response

**Success (200 OK):**
```json
{
  "status": "success",
  "message": "Text updated successfully"
}
```

**Error Responses:**

- **400 Bad Request** - Invalid JSON format:
  ```json
  {
    "status": "error",
    "message": "Invalid JSON format"
  }
  ```

- **400 Bad Request** - Missing text field:
  ```json
  {
    "status": "error",
    "message": "Missing 'text' field"
  }
  ```

- **400 Bad Request** - Text too long:
  ```json
  {
    "status": "error",
    "message": "Text too long"
  }
  ```

- **400 Bad Request** - Invalid text type:
  ```json
  {
    "status": "error",
    "message": "'text' field must be a string"
  }
  ```

#### Examples

**Using curl:**
```bash
# Update display text
curl -X POST http://192.168.1.100/api/scroll \
  -H "Content-Type: application/json" \
  -d '{"text":"Welcome to our store!"}'

# Set a longer message
curl -X POST http://192.168.1.100/api/scroll \
  -H "Content-Type: application/json" \
  -d '{"text":"This is a longer message that will scroll across the display"}'
```

**Using JavaScript (fetch):**
```javascript
// Update display text
fetch('http://192.168.1.100/api/scroll', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    text: 'Hello from JavaScript!'
  })
})
.then(response => response.json())
.then(data => console.log('Success:', data))
.catch((error) => console.error('Error:', error));
```

**Using Python (requests):**
```python
import requests

url = "http://192.168.1.100/api/scroll"
data = {"text": "Hello from Python!"}

response = requests.post(url, json=data)
print(response.json())
```

## Configuration Options

### Text Buffer Size
- Maximum text length: 255 characters
- Buffer size can be modified in `main.cpp` by changing the `text[256]` array size

### Scroll Speed
- Default: 80ms per frame
- Modify `TASK_MILLISECOND * 80` in the `tScroll` task definition to change speed

### WiFi Timeout
- Default: 30 seconds
- Modify `CONNECT_TIMEOUT` constant to change timeout duration

## Troubleshooting

### Common Issues

1. **WiFi Connection Failed**
   - Check WiFi credentials in `config.h`
   - Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
   - Check serial monitor for connection attempts

2. **LED Matrix Not Displaying**
   - Verify pin connections
   - Check power supply to LED matrix
   - Ensure HT1632 library compatibility

3. **API Not Responding**
   - Ensure ESP32 is connected to WiFi
   - Check IP address in serial monitor
   - Verify port 80 is not blocked by firewall

### Debug Mode

The project includes comprehensive debug output. To enable/disable:
- Comment/uncomment `#define _DEBUG_` in `main.cpp`
- Serial output shows WiFi connection status, API requests, and text updates

## Project Structure

```
Sure3208Matrix/
├── include/
│   ├── config.h          # WiFi configuration
│   └── README
├── lib/
│   └── README
├── src/
│   └── main.cpp          # Main application code
├── test/
│   └── README
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## License

This project is open source. Feel free to modify and distribute according to your needs.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and questions:
1. Check the troubleshooting section
2. Enable debug mode and check serial output
3. Review the PlatformIO build output for dependency issues
