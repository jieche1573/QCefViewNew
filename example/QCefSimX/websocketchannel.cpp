#include "websocketchannel.h"

#include "websockettransport.h"
#include <QWebSocketServer>

WebSocketChannel::WebSocketChannel(QWebSocketServer* server)
  : _server(server)
{
  connect(server, &QWebSocketServer::newConnection, this, &WebSocketChannel::handleNewConnection);

  connect(this, &WebSocketChannel::clientConnected, this, &WebSocketChannel::connectTo); // connectTo槽继承自QWebchannel
}

void
WebSocketChannel::handleNewConnection()
{
  emit clientConnected(new WebSocketTransport(_server->nextPendingConnection()));
}
