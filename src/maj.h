/*
 *                        maj.h
 * 
 *  This file is part of COLLATINVS.
 *                                                                            
 *  COLLATINVS is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *                                                                            
 *  COLLATINVS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                            
 *  You should have received a copy of the GNU General Public License
 *  along with COLLATINVS; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MAJ_H
#define MAJ_H

#include <QDialog>
#include <QtGui>
#include <QFile>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QNetworkAccessManager>
#if QT_VERSION >= 0x050000 // Philippe travaille avec Qt5 sur Mac et a besoin de ces 3 lignes !!!
#include <QtWidgets>
#endif

QString const urlDepot = "http://outils.biblissima.fr/collatinus/";

typedef QList<QStringList> ListeF;

class HttpWindow: public QDialog
{
    Q_OBJECT
public:
    HttpWindow (QWidget *parent = 0);
    void append(const QUrl &url);
    void append(const QStringList &urlList);
    QString saveFileName(const QUrl &url);

signals:
    void finished();

private slots:
    void startNextDownload();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();
    void telecharger ();

private:
    void setupUi(QDialog *Dialog);
    // widgets
    QString depot;
    QVBoxLayout *verticalLayout;
    QLabel *urlLabel;
    QPushButton *boutonTelech;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QProgressDialog *progressDialog;
    // téléchargement
    QNetworkAccessManager manager;
    QQueue<QUrl> downloadQueue;
    QNetworkReply *currentDownload;
    QFile output;
    // QTime downloadTime;
    // variables
    QString repRes;
    QString repDic;
    bool finis;
    int downloadedCount;
    int totalCount;
    bool httpRequestAborted;
    // affichage des fichiers à télécharger
    QStringList suffIdxCfg;
    QStringList suffDic;
    ListeF ls (QString d); // établit la liste les fichiers locaux
    QList<QCheckBox*> lcb; // liste des combos
    QStringList invis; // liste des satellites
    void peupleCombos ();
};

#endif
