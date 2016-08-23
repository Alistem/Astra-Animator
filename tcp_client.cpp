#include "tcp_client.h"

QT_USE_NAMESPACE

Tcpclient::Tcpclient(QObject *parent) :
    QObject(parent)
  , m_nNextBlockSize(0)
  , m_pTcpSocket (new QTcpSocket(this))
{

    s_address = "10.10.33.1";
    port = 1024;
    time_out = 100;

    m_pTcpSocket->connectToHost(s_address,port);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError)));
    connect(&con_timer, SIGNAL(timeout()),SLOT(readFinish()));


}

Tcpclient::~Tcpclient()
{
    if(connect_st)
        m_pTcpSocket->close();
    delete m_pTcpSocket;
}

void Tcpclient::slotSendToServer(QByteArray buffer)
{
    m_pTcpSocket->write(buffer);
    buffer.clear();
}

void Tcpclient::slotConnected() {
    connect_st = true;
    emit ok_connect_tcp();
}

void Tcpclient::slotReadyRead()
{
    read_data.clear();
    con_timer.start(time_out);
    for (;;) {   
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint8))
                return;
            m_nNextBlockSize = m_pTcpSocket->bytesAvailable();
        }
        //qDebug()<<"m_nNextBlockSize"<<m_nNextBlockSize;

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize)
            return;

        read_data=m_pTcpSocket->readAll();

        //qDebug()<<"read_data"<<read_data.toHex();

        m_nNextBlockSize = 0;  
    }
}

QByteArray Tcpclient::read()
{
    return read_data;
}

int Tcpclient::write(QByteArray buffer)
{
    slotSendToServer(buffer);
    return buffer.size();
}

void Tcpclient::readFinish()
{
    con_timer.stop();
    emit finish_read();
}

void Tcpclient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                             "The host was not found." :
                             err == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 err == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(m_pTcpSocket->errorString())
                                     );

    emit info(strError);
}
