/*
 *                          irreg.h
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


#ifndef IRREG_H
#define IRREG_H

#include <QObject>
#include <QString>

#include "entree.h"
#include "traits.h"

class Entree;

class Irreg : public QObject
{
    Q_OBJECT
private:
    QString gr;
    QString grq;
    Entree * e;
    Traits * traits;
    bool excl;

public:
//    explicit Irregs(QObject *parent = 0);
    Irreg (QString linea, Entree * entr, QObject *parent = 0);
    virtual ~Irreg ();
    Entree * entree ();
    QString graphie ();
    QString graphieq ();
    QString morpho ();
    bool exclusif ();
    Traits * getTraits ();
    bool homomorphe (Desinence * des);

signals:
    
public slots:
    
};

#endif // IRREG_H
