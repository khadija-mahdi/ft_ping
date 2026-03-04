# ft_ping

A simple ICMP ping utility implementation in C that sends echo requests to a target host and measures network connectivity and latency.

## Features

- **ICMP Echo Requests**: Sends ICMP echo requests to a specified destination
- **Network Statistics**: Tracks packets sent/received, round-trip time (RTT), and packet loss
- **TTL Control**: Set custom Time-To-Live values for packets with the `-t` option
- **Verbose Mode**: Display detailed information about each ping with the `-v` option
- **Signal Handling**: Gracefully handles `SIGINT` (Ctrl+C) to display summary statistics
- **Error Detection**: Identifies and reports ICMP errors (Time Exceeded, Destination Unreachable)

## Requirements

- GCC compiler
- Linux operating system (requires raw socket access)
- `sudo` privileges (raw sockets require elevated permissions)

## Building

```bash
make              # Build the executable
make clean        # Remove object files
make fclean       # Remove object files and executable
make re           # Clean and rebuild
```

## Usage

```bash
sudo ./ft_ping [options] destination
```

### Options

| Option | Description |
|--------|-------------|
| `-v` | Verbose output - shows additional details like ICMP identifier |
| `-t <ttl>` | Set the Time To Live (TTL) for packets (default: system default) |
| `-h, -?` | Display help message |

### Examples

```bash
# Basic ping
sudo ./ft_ping google.com

# Verbose output
sudo ./ft_ping -v 8.8.8.8

# With custom TTL
sudo ./ft_ping -t 64 example.com

# Ping localhost
sudo ./ft_ping 127.0.0.1
```

## Output Format

### Standard Output

```
PING example.com (93.184.216.34) 56(84) bytes of data.
64 bytes from example.com (93.184.216.34): icmp_seq=1 ttl=56 time=45.123 ms
64 bytes from example.com (93.184.216.34): icmp_seq=2 ttl=56 time=44.987 ms
^C
--- example.com ping statistics ---
2 packets transmitted, 2 received, 0.0% packet loss, time 1001.5 ms
```

### Verbose Output

```
64 bytes from example.com (93.184.216.34): icmp_seq=1 ident=12345 ttl=56 time=45.123 ms
```

### Error Handling

```
From router.local (192.168.1.1) icmp_seq=5 Time to live exceeded
```

## Project Structure

```
ft_ping/
├── main.c           # Program entry point
├── init.c           # Initialization and argument parsing
├── ft_ping.c        # Core ICMP ping implementation
├── utils.c          # Utility functions (checksum, timing, signal handling)
├── ft_ping.h        # Header file with type definitions
├── Makefile         # Build configuration
└── README.md        # This file
```

## How It Works

1. **Socket Creation**: Creates a raw ICMP socket with elevated privileges
2. **DNS Resolution**: Resolves the target hostname to an IP address
3. **Packet Construction**: Builds ICMP echo request packets with timestamps
4. **Packet Transmission**: Sends packets at 1-second intervals
5. **Response Reception**: Listens for ICMP echo replies with a 1-second timeout
6. **Statistics Tracking**: Records RTT, packet loss, and errors
7. **Graceful Shutdown**: Displays statistics when interrupted with Ctrl+C

## Statistics Collected

- **Packets Sent/Received**: Total counts of transmitted and received packets
- **Packet Loss**: Percentage of packets lost during transmission
- **RTT (Round-Trip Time)**:
  - Minimum RTT
  - Maximum RTT
  - Average RTT
- **Errors**: Count of ICMP error messages received
- **Total Time**: Duration of the ping session

## Technical Details

- **ICMP Payload Size**: 56 bytes
- **ICMP Header Size**: 8 bytes
- **IP Header Size**: 20 bytes
- **Total Packet Size**: 84 bytes
- **Response Timeout**: 1 second per packet
- **Interval**: 1 second between packets

## Limitations

- Requires root/sudo privileges (raw socket access)
- Linux-specific implementation
- No packet count limit (runs indefinitely until interrupted)
- Basic error handling for network issues

## Exit Codes

- `0`: Normal exit (after Ctrl+C)
- `1`: Argument parsing error, socket creation failure, or DNS resolution failure

## Notes

- The program runs indefinitely until interrupted with `Ctrl+C`
- Each packet is sent with a 1-second interval
- Responses are waited for up to 1 second before moving to the next packet
- The checksum is automatically calculated for each ICMP packet
- The program uses the process ID as the ICMP identifier
