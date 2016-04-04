/*
 *                          desinence.h
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


#ifndef DESINENCE_H
#define DESINENCE_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "traits.h"

class Desinence : public QObject
{
    Q_OBJECT
private:
    QString gr;  // graphie
    Traits * traits;
    QList<int> mo;      // modèle (paradigma)
    int rn;      // numéro de radical (radix_num)
    QString grq; // graphie avec quantités
    bool rare; // Si la désinence est entre ()
    bool V_phage; // Désinence commençant par un - qui indique qu'il faut retirer le v final du radical

public:
//    explicit Desinence(QObject *parent = 0);
    Desinence (QString linea,QObject *parent = 0);
    QString graphie ();
    QList<int> mdl ();
    QString grquant (); // accesseur de grq
//    QStringList modele ();
    int nr ();
    Traits * getTraits ();
    QString morpho ();
    QString doc ();
    bool est_rare (); // accesseur de rare
    bool est_V_phage (); // accesseur de V_phage

signals:
    
public slots:
    
};

#endif // DESINENCE_H
