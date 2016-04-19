/*
 *                          analysemorpho.h
 *                          partie de l'ancien libcollatinus.h
 *
 *  Saucissonné en sept fichiers correspondant aux classes
 *      Philippe Novembre 2015
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


#ifndef ANALYSEMORPHO_H
#define ANALYSEMORPHO_H

#include <QObject>
#include <QStringList>

#include "desinence.h"
#include "entree.h"

class Entree;

class AnalyseMorpho : public QObject
{
    Q_OBJECT
private:
    Entree * e;
    Desinence * des;
    Traits * traits;
    QString q; // devient le seul radical (le 4 mars 2014)
    QString dq; // nécessaire pour le superlatif de l'adverbe
    QString suffixe; // le suffixe sera conservé ici
    bool db_i;
    QString ajoutSuff (QString fq, int accent);
    static QString transforme (QString k);
    static QString accentue (QString l);

    QStringList static const assimAnte;
    QStringList static const assimPost;
    QStringList static const assimPostq;
    QStringList static const assimAnteq;

public:
//    explicit AnalyseMorpho(QObject *parent = 0);
    AnalyseMorpho (QString rq, Desinence * d, Entree * en, QString
                   suff="", bool ii = false,QObject *parent = 0);
    AnalyseMorpho (QString rq, Traits * t, Entree * en, QString ddq="",QObject *parent = 0);
    ~AnalyseMorpho ();
    // pour les indéclinables et la 3ème décl, la
    // forme entière est dans rq.

    static QString allonge (QString f);
    static QString  assimile (QString f, int &i);
    void assimileq (int i);

    QString static const consonnes;
    QString static const voyelles;
    QString static const longues;
    QChar static const separSyll;

    Entree * entree ();
    void setSuff (QString s); // Pour les enclitiques, je dois pouvoir ajouter un suffixe après coup
    QString getSuff (); // récupérer le suffixe.
    QString getD (); // pour récupérer la désinence
    void setQ (QString nq); // pour changer le radical;
    QString getQ (); // pour récupérer le radical
    QString getForm (int accent = 0); // Le collage radical+désinence+suffixe se fera ici
    void setUpper ();
    void setLower ();
    Traits * getTraits ();
    QString humain ();

signals:
    
public slots:
    
};

#endif // ANALYSEMORPHO_H
