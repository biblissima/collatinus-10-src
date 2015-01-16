/*                           lewis.cpp
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
 *  
 */

#include "lewis.h"
#include <QtXmlPatterns>

/****************
 * Dictionnaire *
 ****************/

Dictionnaire::Dictionnaire (QString cfg)
{
    QFileInfo fi(cfg);
    repertoire = qApp->applicationDirPath () + "/ressources/dicos/";
    n = fi.baseName ().section ('.', 0);
    // lire le fichier de ressource cfg
    QSettings settings (repertoire + cfg, QSettings::IniFormat);
    settings.setIniCodec ("utf-8");
    settings.beginGroup ("droits");
    auteur = settings.value ("auteur").toString ();
    url = settings.value ("url").toString ();
    settings.endGroup ();
    settings.beginGroup ("fichiers");
    chData = repertoire + settings.value ("data").toString ();
    debut = settings.value ("debut").toInt ();
    echelle = settings.value ("echelle").toString ();
    if (echelle.isEmpty ()) echelle = "160";
    idxJv = repertoire + n + ".idx";
    settings.endGroup ();
    settings.beginGroup ("remplacements");
    cond_jv = settings.value ("condjv").toString ();
    ji = settings.value ("ji").toInt ();
    JI = settings.value ("JI").toInt ();
    settings.endGroup ();
    settings.beginGroup ("style");
    xsl = settings.value ("xsl").toInt ();
    settings.endGroup ();
    xml = QFileInfo (chData).suffix () == "xml";
    djvu = !xml;
}

QString Dictionnaire::nom ()
{
    return n;
}


QString Dictionnaire::convert (QString source)
{
    QXmlQuery query(QXmlQuery::XSLT20);
    query.setFocus (source);
    QString fichier = repertoire + n + ".xsl";
    QFile xml2html (fichier);
    xml2html.open (QIODevice::ReadOnly|QIODevice::Text);
    query.setQuery (&xml2html);
    QString html, retour;
    QTextStream ts (&retour);
    ts.setCodec ("UTF-8");
    query.evaluateTo (&html);
    ts << html;
    return retour ;
}

QString Dictionnaire::entree_pos (qint64 pos)
{
   QFile file (chData);
   file.open (QFile::ReadOnly | QFile::Text);
   file.seek (pos);
   QTextStream ts (&file);
   ts.setCodec ("UTF-8");
   QString linea = ts.readLine ();
   file.close ();
   if (xsl)
   {
       linea = convert (linea);
       return linea;
   }
   else 
   {
       linea.replace("H1>","strong>");
       linea.prepend("<br/>\n");
       return linea;
   }
   return "Error. Nil legere potui.";
}

void Dictionnaire::vide_index ()
{
    idxDjvu.clear ();
}

void Dictionnaire::vide_ligneLiens ()
{
    ligneLiens.clear ();
}

bool Dictionnaire::lis_index_djvu ()
{
    QFile f (idxJv);
    if (!f.open (QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream ts (&f);
    while (!f.atEnd ())
    {
        idxDjvu << f.readLine ().trimmed ();
    }
    f.close ();
    return true;
}

QString Dictionnaire::pageDjvu (int p)
{
#ifdef Q_OS_MAC
    QString sortie_qt = QDir::homePath () + "/.pagefelix.tif";
    QString sortie_ddjvu = QDir::homePath() + "/.pagefelix.tif";
    QString ddjvu = "/Applications/DjView.app/Contents/bin/ddjvu";
#else
#ifdef Q_OS_WIN32 
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QString sortie_ddjvu = QDir::toNativeSeparators (env.value ("TMP")+ "/pagefelix.tif");
	QString sortie_qt = QDir::tempPath () + "/pagefelix.tif";
#else
    QString sortie_qt = QDir::homePath () + "/.pagefelix.tif";
    QString sortie_ddjvu = QDir::homePath() + "/.pagefelix.tif";
    QString ddjvu = "/usr/bin/ddjvu";
#endif
#endif
	QStringList args;
    args << "-format=tif"
	    << "-page=" + QString::number (p)
	    << "-scale=" + echelle
	    << chData 
	    << sortie_ddjvu; 
    QObject * parent = new QObject;
	QProcess * proc = new QProcess (parent);
    proc->setWorkingDirectory(qApp->applicationDirPath ());
    QString pg;
#ifdef Q_OS_WIN32
    QString ddjvu = "ddjvu.exe";
	proc->start (ddjvu, args);
#else
    if (!QFile::exists(ddjvu))
    {
        QTextStream (&pg) << "<html><h2>Il faut installer DjView.app\n"
                             "dans le dossier Applications.<br>\n"
                             "<a href=\"http://sourceforge.net/projects/djvu/files/DjVuLibre_MacOS/\">\n"
                             "DjVuLibre_MacOS</a></h2></html>";
    }
    {
        proc->start (ddjvu, args);
    }
#endif
    proc->waitForFinished (-1);
    if ((proc->error () == QProcess::ReadError) || (proc->exitCode ()))
		QTextStream (&pg) << "<html><strong>Fichier djvu ou "<< sortie_ddjvu << " introuvable</strong><br>\n"
			"Sous Linux, installer le paquet djview<br>\n"
			"Collatinux X.1 - Licence GNU-GPL</html>";
	else
    {
        QTextStream (&pg) << auteur << " <html><a href=\"" << url <<"\">" << url << "</a> "
            << "<br/>\n" << ligneLiens << "<br/>\n<img src=\"" << sortie_qt << "\"></html>";
    }
    delete proc;
    return pg;
}

QString Dictionnaire::pageDjvu (QStringList req, int no)
{
    // seul le lemme n° noLien est affiché, les autres sont en hyperliens.
    // Si l'index chargé est le bon, inutile de le relire.
    QString leLem = req[no];
    if (idxDjvu.isEmpty ())
    {
        lis_index_djvu ();
    }
    pdj = debut;
    foreach (QString l, idxDjvu)
    {
        if (QString::compare (l, leLem, Qt::CaseInsensitive) > 0)
        {
            break;
        }
        ++pdj;
    }
    ligneLiens.clear ();
    foreach (QString lien, req)
         ligneLiens.append ("<a href=\""+lien+"\">"+lien+"</a>&nbsp;");
    return pageDjvu (pdj);
}

QString Dictionnaire::pageXml (QStringList req)
{
    QString pg; // contenu de la page de retour
    llew listeE;
    QFile * findex = NULL;
    ligneLiens.clear ();
    foreach (QString l, req)
    {
        l = l.toLower ();
        findex = new QFile (idxJv);
        if (ji) l.replace ('j', 'i');
        if (findex == NULL || !findex->open(QFile::ReadOnly | QFile::Text))
        {
            prec = "error";
            suiv = "error";
            return "Error";
        }
        QString linea;
        QStringList eclats;
        QString avanDerCh, derCh, ch;
        bool trouve = false;
        int trouve1 = -10; // on commence avec trouve1 négatif puisque "a" est < l
        int p;
        while (trouve1 < 0) // on s'arrête quand on a dépassé l
        {
            linea = findex->readLine ();
            p = linea.indexOf(":");
            if (p > -1)
            {
                avanDerCh = derCh;
                derCh = ch;
                eclats = linea.split(":");
                ch = eclats[0];
                trouve  = QString::compare (ch, l, Qt::CaseInsensitive ) == 0;
                trouve1 =  QString::compare (ch, l+"1", Qt::CaseInsensitive );
                // trouve1 est un entier qui est la différence entre les chaines
                if (trouve)
                {
                    prec = derCh;
                    if (eclats.size() == 3)
                        listeE.append (pairL (eclats[2].trimmed (), eclats[1].toLongLong ()));
                    else listeE.append (pairL (ch, eclats[1].toLongLong ()));
                    linea = findex->readLine ();
                    eclats = linea.split (":");
                    ch = eclats [0];
                    trouve1 = 1;
                }
                else if (trouve1 == 0)
                {
                    prec = derCh;
                    while (QRegExp ("^"+l+"\\d+$").exactMatch (ch.toLower ()))
                    {
                        if (eclats.size() == 3)
                            listeE.append (pairL (eclats[2].trimmed (), eclats[1].toLongLong ()));
                        else listeE.append (pairL (ch, eclats[1].toLongLong ()));
                        linea = findex->readLine ();
                        eclats = linea.split(":");
                        ch = eclats[0];

                    }
                }
                else if (trouve1 > 0 || findex->atEnd ())
                {
                    QString rl = ramise (l);
                    if (rl != l)
                    {
                        QStringList lramise; 
                        lramise << rl;
                        return pageXml (lramise);
                    }
                    prec = derCh;
                    if (eclats.size() == 3)
                        listeE.append (pairL (eclats[2].trimmed (), eclats[1].toLongLong ()));
                    else listeE.append (pairL (ch, eclats[1].toLongLong ()));
                    break;
                }
                // je ne suis pas sûr que ce dernier else soit nécessaire
            }
        }
        suiv = eclats[0];
        findex->close ();
    }

    int i = 0;
    while (i < listeE.size())
    { 
        if (ligneLiens.contains("<a href=\"#"+listeE[i].first+"\">"))
            listeE.removeAt(i); 
        else
        {
            ligneLiens.append ("<a href=\"#"+listeE[i].first+"\">"+listeE[i].first+"</a>&nbsp;");
            ++i;
        }
    }
    ligneLiens.append ("<br/>\n");
    for (int i=0;i<listeE.size();i++)
    {
        pg.append ("\n<div id=\""+listeE[i].first+"\">");
        pg.append ("<br/>\n" + ligneLiens); 
        QString np = entree_pos (listeE[i].second);
        pg.append (np);
        pg.append ("</div>");
    }
    if (QFile::exists (repertoire + n +  ".css"))
    {
        pg.prepend ("<link rel=\"stylesheet\" href=\""+repertoire+n+".css\" type=\"text/css\" />\n");
    }
    pg.prepend (auteur + " <a href=\"http://"+url+"\">"+url+ "</a> ");
    /*
    // code de débogage
    QFile fdebug ("debug.html");
    fdebug.open (QFile::WriteOnly | QFile::Text);
    QTextStream fl (&fdebug);
    fl << pg; 
    fdebug.close ();
    */
    return pg;
}

QString Dictionnaire::page (QStringList req, int no)
{
    if (xml)
        return pageXml (req);
    else // djvu
    {
        liens = req;
        return pageDjvu (req, no); // passage de req pour les hyperliens
    }
}

bool Dictionnaire::estXml ()
{
   return xml;
}

QString Dictionnaire::pgPrec ()
{
    return prec;
}

QString Dictionnaire::pgSuiv ()
{
    return suiv;
}

int Dictionnaire::noPageDjvu ()
{
    return pdj;
}

QString Dictionnaire::indexJv ()
{ 
    return idxJv;
}

QStringList Dictionnaire::links () 
{
    return liens;
}

/****************
*    ListeDic   * 
*****************/

Dictionnaire * ListeDic::dictionnaire_par_nom (QString nom)
{
    QMap<QString, Dictionnaire*>::iterator retour = liste.find (nom);
    if (retour == liste.end ())
        return NULL;
    return retour.value ();
}

void ListeDic::ajoute (Dictionnaire *d)
{
    liste.insert (d->nom (), d);
}

void ListeDic::change_courant (QString nom)
{
    currens = dictionnaire_par_nom (nom);
}

Dictionnaire * ListeDic::courant ()
{
    return currens;
}

QString Dictionnaire::ramise (QString f)
{
    if (!ji)
        f = f.replace (QRegExp ("(^|[aeo]+|^in|^ad|^per)i([aeiou])"), "\\1j\\2");
    f = f.replace (QRegExp ("(^|[aeio]+|^in|^ad|^per)u([aeiou])"), "\\1v\\2");
    f = f.replace (QRegExp ("(^|[\\w]+r)u([aeiou])"), "\\1v\\2");
    return f;
}

/*
   Copie du fichier téléchargé L&S en xml dans
   ressources ; Création d'un index, à ensuite
   dédoubler en transformant v en u et j en i. Il
   faudra enfin trier les deux par ordre alphabétique,
   et éliminer la première ligne, qui ne correspond à
   aucune entrée.
*/
/*
bool andromeda (QString nf)
{
    QFile fandr (nf);
    if (!fandr.open (QFile::ReadOnly | QFile::Text))
        return false;
    // QFile::copy (nf, qApp->applicationDirPath () + "/ressources/lewis.xml");
    QFile findex (qApp->applicationDirPath () + "/ressources/lewis.idx");
    // QFile::copy (nf, qApp->applicationDirPath () + "/ressources/ducange.xml");
    // QFile findex (qApp->applicationDirPath () + "/ressources/du.idx");
    if (!findex.open (QFile::WriteOnly | QFile::Text))
        return false;
    QString linea;
    QString cle;
    QTextStream fli (&findex);
    fli.setCodec ("UTF-8");
    qint64 p; 
    fandr.seek (0);
    while (!fandr.atEnd ())
    {
       // flux.flush ();
       p = fandr.pos ();
       linea = fandr.readLine ();
       // int pos = exp.indexIn (linea);
       // ducange :
       //QRegExp expr ("(<H1>)([^<]+)(</H1>)");
       //int pos = expr.indexIn (linea);
       //if (pos > -1)
       //{ 
       //    cle = expr.cap (2);
       //   fli << cle << ":" << p  << "\n";
       //}
       
       // pour lewis
       QRegExp exp ("(^.*key=\")([^\"]+)(\".*$)");
       int pos = exp.indexIn (linea);
       if (pos > -1)
       {
           cle = exp.cap (2);
           fli << cle << ":" << p  << "\n";
       }
    }
    fandr.close ();
    findex.close ();
    return true;
}
*/



/*
   // Ramiste
   QStringList exceptions;
   exceptions << "Achaia" << "Aglaia" << "aio" << "ambubaia" << "baia" << "Baiae" 
    << "Caia" << "caiatio" << "Caieta" << "caio" << "Graii" << "Graioceli"
    << "Isaias" << "Laiades" << "maia" << "Panchaia";
*/

/*
// Dans le cas de téléchagement à partir
// d'Andromeda :
    cle.replace (QChar (0x0100), 'A');
    cle.replace (QChar (0x0101), 'a');
    cle.replace (QChar (0x0102), 'A');
    cle.replace (QChar (0x0103), 'a');
    cle.replace (QChar (0x0112), 'E');
    cle.replace (QChar (0x0113), 'e');
    cle.replace (QChar (0x0114), 'E');
    cle.replace (QChar (0x0115), 'e');
    cle.replace (QChar (0x012a), 'I');
    cle.replace (QChar (0x012b), 'i'); 
    cle.replace (QChar (0x012c), 'I');
    cle.replace (QChar (0x012d), 'i');
    cle.replace (QChar (0x014c), 'O');
    cle.replace (QChar (0x014d), 'o');
    cle.replace (QChar (0x014e), 'O');
    cle.replace (QChar (0x014f), 'o');
    cle.replace (QChar (0x016a), 'U');
    cle.replace (QChar (0x016b), 'u');
    cle.replace (QChar (0x016c), 'U');
    cle.replace (QChar (0x016d), 'u');
    // quelques caractères inusités :
    cle.replace (QChar (0x0233), 'y');  
    cle.replace (QChar (0x0304), 'i'); 
    cle.replace (QChar (0x0306), 'i'); 
    cle.replace (QChar (0x5e), "");     // ^
    // non ramiste ! mais traiter les composés
    // de iacio
    if (cle.endsWith ("jicio"))
    {
    cle.replace ("jicio", "icio");
    }
    else
    {
    cle.replace ('j', 'i');
    }
    cle.replace ('J', 'I');
    cle.replace ('v', 'u');
    cle.replace ('V', 'U');
*/
