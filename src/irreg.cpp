/*
 *                          irreg.cpp
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


#include "irreg.h"
/*
Irregs::Irregs(QObject *parent) :
    QObject(parent)
{
}
*/
/********************/
/* Les irréguliers  */
/********************/

Irreg::Irreg (QString linea, Entree * entr, QObject *parent) :
    QObject(parent)
{
    // maximis|magnus|5|1|2|0|0|0|0|0
    // gr        k    c g n d p t m v
    QStringList eclats = linea.split ('|');
    int ec = eclats.count();
    if (ec != 10 && ec != 3)
    {
#ifdef DEBOG
        qDebug () << "irregs.la" << linea << "ligne mal formée";
#endif
        return;
    }
    grq = eclats[0];
    excl = grq.endsWith ('*');
    if (excl)
        grq.chop (1);
    gr = Ch::deramise (grq);
    e = entr;
    if (eclats.count () > 9)
    {
    traits = new Traits (eclats[2].toInt (),
                         eclats[3].toInt (),
                         eclats[4].toInt (),
                         eclats[5].toInt (),
                         eclats[6].toInt (),
                         eclats[7].toInt (),
                         eclats[8].toInt (),
                         eclats[9].toInt ());
    }
    else
        traits = new Traits (eclats.at (2));
}

Irreg::~Irreg ()
{
   delete traits;
}

Entree * Irreg::entree ()
{
    return e;
}

QString Irreg::graphie ()
{
    return gr;
}

QString Irreg::graphieq ()
{
    return grq;
}

QString Irreg::morpho ()
{
    return traits->humain ();
}

Traits * Irreg::getTraits ()
{
    return traits;
}

bool Irreg::exclusif ()
{
    return excl;
}

bool Irreg::homomorphe (Desinence * des)
{
    return traits->getBits () == des->getTraits ()->getBits ();
}
