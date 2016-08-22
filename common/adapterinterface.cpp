#include "adapterinterface.h"

QT_USE_NAMESPACE

AdapterInterface::AdapterInterface(QString type)
    : IAdapterInterface(type),port_ok(0), mCom(NULL), mTcp(NULL)
{
    type_connection = type.toInt(0);
}

AdapterInterface::~AdapterInterface()
{
    if (mCom)
        delete mCom;
    mCom = NULL;
    if (mTcp)
        delete mTcp;
    mTcp = NULL;
}

void AdapterInterface::port(QString num)
{
    port_num = num;
}

void AdapterInterface::portConnect()
{
    switch (type_connection) {
    case 1:
        //qDebug()<<"mCom";
        mCom = new ComPort(port_num);
        connect(mCom,SIGNAL(finish_read()),SIGNAL(finish_read_adapter()));
        if(mCom->portOpen())
            ok_connect_st();
        break;
    case 2:
        //qDebug()<<"mTcp";
        mTcp = new Tcpclient(this);
        connect(mTcp,SIGNAL(ok_connect_tcp()),SLOT(ok_connect_st()));
        connect(mTcp,SIGNAL(finish_read()),SIGNAL(finish_read_adapter()));
        break;

    default:
        break;
    }
}



void AdapterInterface::ok_connect_st()
{ 
    port_ok = 1;
    qDebug()<<"portOpen"<<port_ok;
    emit ok_connect();
}

bool AdapterInterface::portOpen()
{
    return port_ok;
}

QByteArray AdapterInterface::read()
{
    QByteArray buffer;

    switch (type_connection) {
    case 1:
        buffer = mCom->read();
        break;
    case 2:
        buffer = mTcp->read();
        break;

    default:
        break;
    }
    return buffer;
}

int AdapterInterface::write(QByteArray data)
{
    switch (type_connection) {
    case 1:
        mCom->write(data);
        break;
    case 2:
        mTcp->write(data);
        break;

    default:
        break;
    }
    return data.size();
}


