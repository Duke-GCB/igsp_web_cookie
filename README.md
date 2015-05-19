igsp\_web\_cookie
===============

Daemon and Command-line tools for the IgspNet Authentication system.

## Overview

This repository includes a trio of command-line tools that create and verify [cookies](http://en.wikipedia.org/wiki/HTTP_cookie) for Single Sign-On across GCB (Formerly IGSP) web applications.

## Build

### Dependencies:

#### From Oracle:

- Oracle Instant Client & SDK (or full client if installing on oracle server)
  - Tested with instantclient 10 and 11.
  - Make sure `ORACLE_HOME` is set to the directory where the instantclient or full client is installed.

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

A Makefile is included in the git repo. After cloning, type `make`. Binaries are created in the `bin` directory. Three binaries are built: `cookieDaemon`, `signCookie`, and `verifyCookie`.

    $ git clone https://github.com/Duke-GCB/igsp_web_cookie.git
    $ cd igsp_web_cookie
    $ make

## Configuration

__Note__: if you are installing igsp\_web\_cookie to join an existing IGSPNet environment, you must use the same key/certificate and connect to the same database. Cookies generated with one key/cert cannot be verified with another key/cert.

Generate a private key and certificate (public key) in PEM format, using [OpenSSL](https://www.openssl.org). `signCookie` will use the private key to digitally sign the cookie, and `verifyCookie` will the public key to verify the signature.

    # Generate a 4096-bit private key in key.pem
    openssl genrsa -out key.pem 4096

    # Generate a certificate/public key from the private key in cert.pem, valid for 10 years
    openssl req -new -x509 -key key.pem -out cert.pem -days 3650

Write a Config file, using [cookied-example.conf](cookied-example.conf) as a template.

- `SOCKET_PATH`: `verifyCookie` talks to `cookieDaemon` over a socket. Specify the path to the socket on the filesystem to use here. `cookieDaemon` will make and remove this socket, so the directory must exist and must be writable to the user that runs `cookieDaemon`
- `DB_CONN_STRING`: The Oracle connection string for OCCI
- `DB_USER`: The Oracle account username to connect as
- `DB_PASS`: The password for the above account
- `PRIVATE_KEY_PATH`: The path to the PEM-formatted private key
- `CERT_PATH`: The path to the PEM-formatted certificate

Remember, this file contains database credentials, so protect it on your host. Also be sure to protect the private key file so that only the user that runs `signCookie` can read it.

## Installation

1. Make a directory for installation. This should be writable by the user that will run the binaries (possibly apache or oracle)

        mkdir -p /var/system/cookied

2. cd to the directory where igsp\_web\_cookie was built run make install. This places the binaries in the above directory:

        make install prefix=/var/system/cookied

3. Place config file and keys in the `COOKIE_DAEMON_CONFIG` directory (see [Configuration](#configuration)).
4. Create an init script for `cookied`. See [cookied.j2](https://github.com/Duke-GCB/gcb-ansible/blob/master/roles/igsp_web_cookie/templates/cookied.j2) as a template. You'll need to fill in values for anything between `{{ }}`, including the above install directory as well as the path to your `cookied.conf` file
5. Install, enable, and start the `cookied` service (These are chkconfig instructions, adapt to your environment as needed)

        sudo cp cookied /etc/init.d/
        sudo chmod 0755 /etc/init.d/cookied
        sudo chkconfig --add cookied
        sudo chkconfig cookied on
        sudo service cookied start

6. Confirm installation by signing and verifying a cookie. See [Examples](#examples).

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
