# libsoup_websocket_client
A demo websocket client using libsoup (including wss)

## How to build
Use Scons to build

$ scons

## How to use:
$ ./websocket_client -h

Application Options:

  -a, --address     Websocket server address (default: echo.websocket.org)

  -p, --port        Websocket server port (default: 80)

  -s, --is_wss      Enable wss connection (default: disabled)

## Dependencies
* glib-2.0
* libsoup-2.4
