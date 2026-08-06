*This project has been created as part of the 42 curriculum by oait-h-m, sboukiou, hmouis.*

# ft_irc

## Description

**ft_irc** is an Internet Relay Chat (IRC) server written in **C++98**. The objective of the project is to implement the core features of the IRC protocol while handling multiple clients simultaneously using non-blocking sockets and `poll()`.

The server allows clients to authenticate, join and create channels, exchange private and channel messages, manage channel modes, and interact through standard IRC commands. The implementation follows the IRC protocol specifications and is compatible with IRC clients such as **HexChat**.

As part of the bonus, the project also includes an IRC bot capable of responding automatically to predefined commands.

### Features

- Non-blocking server using `poll()`
- Multiple simultaneous client connections
- Password authentication
- Channel creation and management
- Private and channel messaging
- Channel operators and permissions
- Channel modes (`i`, `t`, `k`, `l`, `o`)
- IRC numerics and protocol-compliant replies
- IRC Bot (Bonus)

### Implemented Commands

- PASS
- NICK
- USER
- JOIN
- PART
- PRIVMSG
- NOTICE
- INVITE
- KICK
- TOPIC
- MODE
- WHO
- QUIT
- PING / PONG

### Bonus

The project includes an IRC bot implemented in C++98.

Available commands:

- `!help`
- `!ping`
- `!time`

---

## Instructions

### Compilation

```bash
make
```

Build the bonus:

```bash
cd bonus
make
```

### Run the server

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 1234
```

### Run the bot (Bonus)

```bash
./bot <port> <password>
```

Example:

```bash
./bot 6667 1234
```

---

## Technical Choices

- Language: **C++98**
- I/O multiplexing with `poll()`
- BSD sockets
- Modular command architecture
- STL containers for managing clients and channels
- RFC-compliant message formatting and numeric replies

---

## Resources

### IRC Documentation

- RFC 1459 – Internet Relay Chat Protocol
- RFC 2810 – IRC Architecture
- RFC 2811 – Channel Management
- RFC 2812 – Client Protocol
- ircdocs: https://modern.ircdocs.horse/

## Authors

- Oussama AIT H'MAD (`oait-h-m`)
- Said BOUKIOUD (`sboukiou`)
- Hodaifa MOUIS (`hmouis`)
