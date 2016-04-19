#include <QCoreApplication>
#include <iostream>
#include <QtWidgets>
#include <QtNetwork>

class QTcpSocket;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (argc > 1)
    {
        QString req (argv[1]);
        int i = 2;
        while (i < argc)
        {
            QString suite(argv[i]);
            req += " " + suite;
            i++;
        }
        QTcpSocket * tcpSocket = new QTcpSocket();
        tcpSocket->abort();
        tcpSocket->connectToHost(QHostAddress::LocalHost, 5555);
        QByteArray ba = req.toUtf8();
        tcpSocket->write(ba);
        tcpSocket->waitForBytesWritten();
        tcpSocket->waitForReadyRead();
        ba = tcpSocket->readAll();
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
        QString rep(ba);
        std::cout << rep.toStdString();
    }
    a.quit();
}

