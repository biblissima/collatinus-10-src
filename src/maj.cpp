/*
 *                        maj.cpp
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

#include "maj.h"

#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <stdio.h>

HttpWindow::HttpWindow (QWidget *parent)
    : QDialog (parent)
{
    progressDialog = new QProgressDialog (this);
    setupUi (this);
    // chemins
    repRes = qApp->applicationDirPath () + "/ressources/";
    repDic = repRes + "dicos/";
    // liste des extensions dictionnaire
    suffDic.clear (); // en cas de réouverture du dialogue
    invis.clear ();   // idem
    suffIdxCfg << "idx" << "cfg" << "xsl" << "css";
    suffDic << "djvu" << "xml" << suffIdxCfg; 
    // télécharger l'url du catalogue
    append (QStringList () << urlDepot + "url_depot.txt");
    while (!finis) 
    {
        qApp->processEvents (); 
    }
    QFile *fdepot = new QFile (repRes + "url_depot.txt");
    fdepot->open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (fdepot);
    depot = flux.readAll ().trimmed ();
    fdepot->close ();
    delete fdepot;
    // télécharger le catalogue
    append (QStringList () << depot + "catalogue.txt");
}

void HttpWindow::setupUi (QDialog *Dialog)
{
    Dialog->setObjectName (QString::fromUtf8 ("Collatinus - mise à jour"));
    setModal (true);
    verticalLayout = new QVBoxLayout (Dialog);
    label = new QLabel (Dialog);
    label->setText (tr ("<strong>Ajouter ou t&eacute;l&eacute;charger.\nIl faudra ensuite relancer Collatinus</strong>"));
    verticalLayout->addWidget (label);
    buttonBox = new QDialogButtonBox (Dialog);
    buttonBox->setOrientation (Qt::Horizontal);
    boutonTelech = new QPushButton (tr ("&Expromere"));
    buttonBox->addButton (boutonTelech,  QDialogButtonBox::ActionRole);
    buttonBox->setStandardButtons (QDialogButtonBox::Close);
    verticalLayout->addWidget (buttonBox);

    QObject::connect (buttonBox, SIGNAL (rejected()), Dialog, SLOT (reject()));
    QObject::connect (boutonTelech, SIGNAL (clicked ()), Dialog, SLOT (telecharger ()));
    QMetaObject::connectSlotsByName (Dialog);
}

void HttpWindow::append (const QStringList &urlList)
{
    foreach (QString url, urlList)
    {
        append (QUrl (url));
        finis = false;
    }
    if (downloadQueue.isEmpty())
    {
        QTimer::singleShot(0, this, SIGNAL(finished()));
    }
}

void HttpWindow::append(const QUrl &url)
{
    if (downloadQueue.isEmpty())
        QTimer::singleShot(0, this, SLOT(startNextDownload()));

    downloadQueue.enqueue(url);
    ++totalCount;
}

QString HttpWindow::saveFileName(const QUrl &url)
{
    // extraire le nom du fichier, et le chemin de
    // destination : ressources ou ressources/dicos.
    QString path = url.toString ();
    QString basename = path.section ('/', -1);
    if (basename.isEmpty())
        basename = "sine_nomine";
    if (suffDic.contains (QFileInfo (basename).suffix ()))
        basename.prepend (repDic);
    else basename.prepend (repRes);
    return basename;
}

void HttpWindow::startNextDownload()
{
    if (downloadQueue.isEmpty()) 
    {
        emit finished();
        finis = true;
        peupleCombos ();
        return;
    }
    finis = false;
    progressDialog->show ();
    QUrl url = downloadQueue.dequeue ();
    QString filename = saveFileName (url);
    output.setFileName (filename);
    if (!output.open(QIODevice::WriteOnly)) 
    {
        fprintf(stderr, "Problème d'ouverture '%s' pour télécharger '%s': %s\n",
                qPrintable(filename), url.toEncoded().constData(),
                qPrintable(output.errorString()));
        startNextDownload();
        return;                 // skip this download
    }
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    currentDownload = manager.get(request);
    connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload, SIGNAL(finished()),
            SLOT(downloadFinished()));
    connect(currentDownload, SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));
    // prepare the output
    // printf("Downloading %s\n", url.toEncoded().constData());
    // downloadTime.start();
}

void HttpWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (httpRequestAborted)
        return;
    progressDialog->setMaximum(bytesTotal);
    progressDialog->setValue(bytesReceived);
}

void HttpWindow::downloadFinished ()
{
    progressDialog->hide ();
    output.close();
    if (currentDownload->error()) 
    {
        // download failed
        fprintf(stderr, "Failed: %s\n", qPrintable(currentDownload->errorString()));
    } 
    currentDownload->deleteLater();
    startNextDownload();
}

ListeF HttpWindow::ls (QString d)
{
    QDir dir (d);
    QFileInfoList infoList = dir.entryInfoList ();
    ListeF lf;
    foreach (QFileInfo fileInfo, infoList)
    {
        if (fileInfo.isDir ())
            continue;
        QString octets;
        octets.setNum (fileInfo.size ());
        lf << (QStringList () << fileInfo.fileName () << octets);
    }
    return lf;
}


void HttpWindow::downloadReadyRead()
{
    output.write (currentDownload->readAll());
}

void HttpWindow::peupleCombos ()
{
    QFile *fcatalogue = new QFile (repRes + "catalogue.txt");
    fcatalogue->open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (fcatalogue);
    QString tout = flux.readAll ();
    fcatalogue->close ();
    delete fcatalogue;

    QStringList ll = tout.split ("\n");
    ListeF lfd;  // liste distants
    foreach (QString l, ll) 
    {
        if (l.length () > 4)
        {
            lfd << l.split (":");
        }
    }
    // vider les cases à cocher;
    foreach (QCheckBox *cb, lcb)
    {
        verticalLayout->removeWidget (cb);
        delete cb;
    }
    lcb.clear ();

    // établir la liste des fichiers locaux dans ./ressources/
    ListeF lfl;  // liste locaux
    lfl << ls (repRes);
    lfl << ls (repDic);
    bool present = false;
    foreach (QStringList ld, lfd)
    { 
        present = false;
        foreach (QStringList ll, lfl)
        {
            // comparaison des noms 
            if (ld[0] == ll[0])
            {
                present = true;
                // comparaison des tailles
                if (ld[1] != ll[1])
                {
                    // si le fichier est visible
                    if (ld.count () > 2)
                    {
                        QCheckBox *cb = new QCheckBox ("Mettre à jour " + ld[2]);
                        cb->setObjectName (ld[0]);
                        verticalLayout->insertWidget (1, cb);
                        lcb << cb;
                    }
                    else invis << ld[0];
                }
            }
        }
        if (!present)
        {
            // fichier distant absent en local
            if (ld.count () > 2)
            {
                QCheckBox *cb = new QCheckBox ("Ajouter " + ld[2]);
                cb->setObjectName (ld[0]);
                verticalLayout->insertWidget (1, cb);
                lcb << cb;
            }
            else invis << ld[0];
        }
    }
}

void HttpWindow::telecharger ()
{
    QStringList liste;
    foreach (QCheckBox * cb, lcb)
    {
        if (cb->isChecked ())
        {
            QString nom = cb->objectName ();
            QFileInfo finfo (nom); 
            QString suff = finfo.suffix ();
            // ajouter les satellites du dictionnaire
            if ((QStringList () << "djvu" << "xml").contains (suff))
            {
                foreach (QString f, invis)
                {
                    QFileInfo finfo2 (f);
                    if (finfo.baseName () == finfo2.baseName ()
                        && suffIdxCfg.contains (finfo2.suffix ())) 
                    {
                        liste << (depot+f);
                    }
                }
            }
            liste << (depot + nom);
        }
    }
    append (liste);
}
