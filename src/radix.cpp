/*
 *                          radix.cpp
 *                          partie de l'ancien libcollatinus.cpp
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


#include "radix.h"
/*
Radix::Radix(QObject *parent) :
    QObject(parent)
{
}
*/

/* classe des radicaux */

Radix::Radix (Entree *entree, int m, int n, QString q, QObject *parent) :
    QObject(parent)
{
    e = entree;
    k = entree->canon ();
    mo = m;
    num = n;
    eni = false;
//    eni = ii;
    grq = q;
}
/*
void Radix::ajouteEntree (Entree *entree)
{
    e = entree;
}
*/
Radix::~Radix ()
{

}

int Radix::mdl ()
{
    return mo;
}

bool Radix::en_i ()
{
    return eni;
}

int Radix::nr ()
{
    return num;
}

Entree * Radix::entree ()
{
    return e;
}

QString Radix::canon ()
{
    return k;
}

QString Radix::grquant ()
{
    return grq;
}

QString Radix::doc ()
{
    QString sortie;
    QTextStream (&sortie)
        << "rad. " << num << " de " << " (" << Entree::modeles[mo] << ")";
    return sortie;
}
