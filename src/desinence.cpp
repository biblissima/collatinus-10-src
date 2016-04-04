/*
 *                          desinence.cpp
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


#include "desinence.h"
/*
Desinence::Desinence(QObject *parent) :
    QObject(parent)
{
}
*/
/*********************
 *                  *
 *    DESINENCES    *
 *                  *
 ********************/

Desinence::Desinence (QString linea, QObject *parent) :
    QObject(parent)
{
    //gr|c|g|n|d|p|t|m|v|mo|rn|grq
    // 0 1 2 3 4 5 6 7 8 9  10 11
    //or|0|0|1|0|1|1|1|2|17|1|ŏr
    QStringList eclats = linea.split('|');
    gr = eclats[0];

    // Ajout de deux propriétés à la désinence
    // Philippe Novembre 2015
    rare = false;
    V_phage = false;
    // Si la désinence est entre parenthèses, elle est rare.
    if (gr.startsWith("(") && gr.endsWith(")"))
    {
        rare = true;
        gr = gr.mid(1,gr.size() - 2);
    }
    // Si la désinence commence par -, elle supprime le v du parfait
    if (gr.startsWith("-"))
    {
        V_phage = true;
        gr = gr.mid(1,gr.size() - 1);
    }

    if (eclats.count() > 5)
    {
        // Format habituel de desin.la
        foreach (QString em, eclats[9].split (','))
            mo << em.toInt ();
        rn = eclats[10].toInt ();
        if (eclats.count () > 11)
        {
            grq = eclats[11];
        }
        else
        {
            grq = gr;
        }
        traits = new Traits (eclats[1].toInt (),
                eclats[2].toInt (),
                eclats[3].toInt (),
                eclats[4].toInt (),
                eclats[5].toInt (),
                eclats[6].toInt (),
                eclats[7].toInt (),
                eclats[8].toInt ());
    }
    else
    {
        // Nouveau format de desin.la basé sur le postag
        // Proposé par Yves en Novembre 2015
        foreach (QString em, eclats[2].split (','))
            mo << em.toInt ();
        rn = eclats[3].toInt ();
        if (eclats.count () > 3)
        {
            grq = eclats[4];
        }
        else
        {
            grq = gr;
        }
        traits = new Traits (eclats.at (1));
    }
}

bool Desinence::est_rare () // accesseur de rare
{
    return rare;
}

bool Desinence::est_V_phage () // accesseur de V_phage
{
    return V_phage;
}

QString Desinence::graphie ()
{
    return gr;
}

QString Desinence::grquant ()
{
    return grq;
}

QList <int> Desinence::mdl ()
{
    return mo;
}

/*
// Tellement équivalent au précédent que cela ne vaut pas
// la peine d'introduire modeles[m] à ce niveau.
QStringList Desinence::modele ()
{
    QStringList ret;
    foreach (int m, mo)
        ret << modeles[m];
    return ret;
}
*/
int Desinence::nr ()
{
    return rn;
}

QString Desinence::morpho ()
{
    return traits->humain ();
}

Traits * Desinence::getTraits ()
{
    return traits;
}
