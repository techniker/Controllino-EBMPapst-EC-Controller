# Simple Controllio-based EBMPapst EC Fan Controller

## Overview

A simple yet effective controller for EBMPapst EC fans, implemented using a Controllino and equipped with an integrated web server. 
This allows for remote control and monitoring of fan speeds via a user-friendly web interface. 
The controller is suitable for applications requiring reliable and precise fan speed control.

## Features

- **Web-Server Integration**: Built-in web server for easy access and control over the network.
- **PWM Control**: Adjust the fan speed using Pulse Width Modulation (PWM) for precise control.
- **User-Friendly Interface**: A clean and simple web interface, including a dark mode toggle for user comfort.
- **Real-time Feedback**: Serial output for monitoring current fan speed and system status.

## Hardware Requirements

- Controllino (or compatible Arduino-based controller)
- EBMPapst EC Fan or any Fan that has a 0-100% duty cycle PWM input
- Ethernet connection

## Software Dependencies

- SPI Library
- Ethernet Library
- Controllino Library

## Installation

1. **Set up the hardware**: Connect the EBMPapst EC fan to the Controllino according to the fan's specifications.
2. **Configure Network Settings**: Modify the `mac` and `ip` variables in the code to suit your network configuration.
3. **Upload the Code**: Use the Arduino IDE to upload the provided code to the Controllino.
4. **Access the Web Interface**: Once the Controllino is online, access the web interface using the IP address shown in the serial monitor.

## Usage

- **Adjust Fan Speed**: Use the web interface to set the desired PWM value (0-100%) for the fan.
- **Quick Set Options**: Choose from predefined PWM values for quick adjustments.
- **Dark Mode**: Toggle between light and dark themes for the web interface.

## Contributing

Contributions, issues, and feature requests are welcome. Feel free to check [issues/requests](#) or send a pull request.

## License

This project is licensed under the [MIT License](LICENSE).