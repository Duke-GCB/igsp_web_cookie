igsp\_web\_cookie
===============

Daemon and Command-line tools for the IgspNet Authentication system.

## Overview

This repository includes a trio of command-line tools that create and verify [cookies](http://en.wikipedia.org/wiki/HTTP_cookie) for Single Sign-On across GCB (Formerly IGSP) web applications.

## Build

### Dependencies:

#### From Oracle:

- Oracle Instant Client & SDK (or full client if installing on oracle server
  - Tested with instantclient 10 and 11.
  - Make sure `ORACLE_HOME` is set to this directory

            export ORACLE_HOME=/usr/local/instantclient_11_2

  - You may need to create symlinks for shared libraries (e.g.):

            ln -s libclntsh.so.11.1 libclntsh.so
            ln -s libocci.so.11.1 libocci.so

#### yum packages:

- gcc-c++
- make
- openssl-devel
- compat-libstdc++-33 (Only for instantclient 10)
- libaio

If you are using instantclient 10, you **must** have `libstdc++.so.5` on your system. This library is not installed by default on current Linux systems (e.g. RHEL/CentOS 7), but is  available from yum in `compat-libstdc++-33`.

The Makefile will detect if you are using instantclient 10 and use this file, instead of the current `libstdc++.so.6`. Linking with `libstdc++.so.6` is very crashy, as they are not [ABI-compatible](https://gcc.gnu.org/onlinedocs/libstdc++/manual/abi.html).

#### Compiling

A Makefile is provided. Type `make`. Binaries are created in the `bin` directory. Three binaries are built: `cookieDaemon`, `signCookie`, and `verifyCookie`.

## Installation

__TODO__

## Configuration

Write a Config file, using [cookied-example.conf](cookied-template.conf) as a template.

- `SOCKET_PATH`: `verifyCookie` talks to `cookieDaemon` over a socket. Specify the path to the socket on the filesystem to use here. `cookieDaemon` will make and remove this socket, so the directory must exist and must be writable to the user that runs `cookieDaemon`
- `DB_CONN_STRING`: The Oracle connection string for OCCI.
- `DB_USER`: The Oracle account username to connect as
- `DB_PASS`: The password for the above account

Remember, this file contains database credentials, so protect it on your host.

## Running

At runtime, all 3 binaries look for the location of the conf file in the environment variable `COOKIE_DAEMON_CONFIG`. Also, `ORACLE_HOME` must be set, and `LD_LIBRARY_PATH` should be updated to include `ORACLE_HOME`.

### Examples

1. Create a cookie:

        $ LD_LIBRARY_PATH=$ORACLE_HOME \
            COOKIE_DAEMON_CONFIG=/var/system/cookied/cookied.conf \
            ./signCookie user123 127.0.0.1 7200 7200

        user123::127.0.0.1 7200::1::ABBAB:::32...

2. Start cookieDaemon from the command-line (not via init script):

        $ LD_LIBRARY_PATH=$ORACLE_HOME \
            COOKIE_DAEMON_CONFIG=/var/system/cookied/cookied.conf \
            ./cookieDaemon &

        [1] 1241

3. Verify a cookie

        $ LD_LIBRARY_PATH=$ORACLE_HOME \
            COOKIE_DAEMON_CONFIG=/var/system/cookied/cookied.conf \
            ./verifyCookie user123::127.0.0.1 7200::1::ABBAB:::32...

        7200

Of course, the environment variables could be exported before running the above commands.
