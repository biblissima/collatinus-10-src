/*
 *                          radix.h
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


#ifndef RADIX_H
#define RADIX_H

#include <QObject>
#include <QString>
#include <QTextStream>

#include "entree.h"

class Entree;

class Radix : public QObject
{

    Q_OBJECT

private:
    Entree * e;
    int mo;    // modèle
    int num;   // numéro de radical
    bool eni;
    QString k; // canon
    QString grq;

public:
//    explicit Radix(QObject *parent = 0);
    Radix (Entree *entree, int m, int n, QString q, QObject *parent = 0);
    virtual ~Radix ();
    int mdl ();
    bool en_i ();
    int nr ();
//    int nbl (); // nombre de langues pointées par private:e
    Entree * entree ();
//    void ajouteEntree (Entree *entree);
    QString canon ();
    QString grquant (); // accesseur de grq
    QString doc ();
};

#endif // RADIX_H
