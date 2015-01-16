/*                           lewis.h
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

#ifndef LEWIS_H
#define LEWIS_H
#include <QtCore>

typedef QPair <QString, qint64> pairL; 
typedef QList <pairL> llew;

class Dictionnaire: public QObject
{
    Q_OBJECT

    private:
        QString n; // nom
        QString repertoire;
        QString chData; // chemin complet du fichier djvu ou xml
        bool xml;
        bool djvu;
        QString auteur;
        QString url;
        int debut;
        QStringList idxDjvu;
        QString idxJv; // chemin de l'index jv
        QString cond_jv;
        bool ji;
        bool JI;
        bool xsl;
        QString prec; // pages précédente et suivante
        QString suiv;
        int pdj;
        bool jv;
        QStringList liens;
        QString ligneLiens;
        QString echelle; // échelle pour l'extraction d'une image d'un djvu
    public:
        Dictionnaire (QString cfg);
        QString nom ();
        void vide_index ();
        void vide_ligneLiens ();
        bool lis_index_djvu ();
        QString convert (QString source);
        QString entree_pos (qint64 pos);
        QString page (QStringList req, int no=0); // lien si la requête vient d'un hyperlien djvu
        QString pageXml (QStringList req);
        QString pageDjvu (int p);
        QString pageDjvu (QStringList req, int no=0); // surcharge avec calcul des no de page
        int noPageDjvu ();
        QString pgPrec ();
        QString pgSuiv ();
        bool estXml ();
        QString indexJv ();
        QString indexIu ();
        QStringList links (); // renvoie liens
        QString ramise (QString f);
};

class ListeDic: public QObject
{
    Q_OBJECT

    private:
    QMultiMap<QString, Dictionnaire*> liste;
    Dictionnaire * currens;

    public:
    Dictionnaire * dictionnaire_par_nom (QString nom);
    void ajoute (Dictionnaire *d);
    void change_courant (QString nom);
    Dictionnaire * courant ();
    // page renvoie le code html de la page 
    // du dictionnaire courant 
    // correspondant au lemme l.
};

#endif

