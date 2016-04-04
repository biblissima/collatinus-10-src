/*
 *                          traits.cpp
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


#include "traits.h"
/*
Traits::Traits(QObject *parent) :
    QObject(parent)
{
}
*/

Traits::Traits (int k, int g, int n, int d, int p, int t, int m, int v, QObject *parent) :
    QObject(parent)
{
    bits.resize (42); // Ajout des locatif et supin 40 -> 42
    bits.setBit (k);
    bits.setBit (g+8);
    bits.setBit (n+12);
    bits.setBit (d+15);
    bits.setBit (p+19);
    bits.setBit (t+23);
    bits.setBit (m+30);
    bits.setBit (v+39); // Ajout du supin 37 -> 38 + 1 pour le locatif
}

Traits::Traits (QString postag, QObject *parent) :
    QObject(parent)
{
    bits.resize (42);
    bits.setBit (refP.at (1).indexOf (postag.at(1))+19); // personne
    bits.setBit (refP.at (2).indexOf (postag.at(2))+12); // nombre
    bits.setBit (refP.at (3).indexOf (postag.at(3))+23); // temps
    bits.setBit (refP.at (4).indexOf (postag.at(4))+30); // mode
    bits.setBit (refP.at (5).indexOf (postag.at(5))+39); // voix
    bits.setBit (refP.at (6).indexOf (postag.at(6))+8);  // genre
    bits.setBit (refP.at (7).indexOf (postag.at(7)));    // cas
    bits.setBit (refP.at (8).indexOf (postag.at(8))+15); // degré
}

const QStringList Traits::cass = QStringList ()
    << "" << "nom." << "uoc." << "acc."
    << "gen." << "dat." << "abl." << "loc.";

const QStringList Traits::nombres = QStringList ()
        << "" << "sing." << "plur.";

const QStringList Traits::personnes = QStringList ()
        << "" << "prim." << "sec." << "tert.";

const QStringList Traits::degres = QStringList ()
        << "" << "posit." << "compar." << "superl.";

const QStringList Traits::modes = QStringList ()
    << "" << "ind." << "coniunct." << "imper." << "infin." // 0 - 4
    <<"part." << "gerund." << "adiect. verb." << "supin";           // 5 - 7

const QStringList Traits::voixs = QStringList ()
    <<"" << "act." << "pass.";

const QStringList Traits::genres = QStringList ()
    << "" << "masc." << "fem." << "neut.";

const QStringList Traits::tempss = QStringList ()
    << "" << "praes." << "fut." << "imperf." << "perf."
    << "fut. ant." << "plus-quam-perf.";

const QStringList Traits::refP = QStringList ()
    << "-vtadcrpmnieu"
    << "-123"
    << "-sp"
    << "-pfirtl"
    << "-ismnpgdu"
    << "-ap"
    << "-mfn"
    << "-nvagdbl"
    << "--cs";

Traits::~Traits ()
{
}

QBitArray Traits::getBits ()
{
    return bits;
}

QString Traits::humain ()
{
    QStringList retour;
    for (int i=0;i<42;++i) // Ajout des locatif et supin 40 -> 42
    {
        if (bits.testBit (i))
        {
                 if (i < 8)  retour << cass[i];
            else if (i < 12) retour << genres[i-8];
            else if (i < 15) retour << nombres[i-12];
            else if (i < 19) retour << degres[i-15];
            else if (i < 23) retour << personnes[i-19];
            else if (i < 30) retour << tempss[i-23];
            else if (i < 39) retour << modes[i-30];
            else  retour << voixs[i-39]; // Ajout du supin 37 -> 38
        }
    }
    return retour.join (" ").simplified ();
}

// retourne le premier cas trouvé
int Traits::casus ()
{
    for (int i=0;i<8;++i)
        if (bits.testBit (i))
            return i;
    return 0;
}

/*
QString Traits::cas ()
{
    QStringList retour;
    for (int i=0;i<7;++i)
        if (bits.testBit (i))
            retour << cass[i];
    return retour.join (" ");
}
*/

int Traits::genus ()
{
    for (int i=8;i<12;++i)
        if (bits.testBit (i))
            return i-8;
    return 0;
}

/*
QString Traits::genre ()
{
    QStringList retour;
    for (int i=7;i<11;++i)
        if (bits.testBit (i))
            retour << genres[i-7];
    return retour.join (" ");
}
*/

/*
QString Traits::nombre ()
{
    QStringList retour;
    for (int i=11;i<14;++i)
        if (bits.testBit (i))
            retour << nombres[i-11];
    return retour.join (" ");
}
*/

int Traits::numerus ()
{
    for (int i=12;i<15;++i)
        if (bits.testBit (i))
            return i-12;
    return 0;
}

/*
QString Traits::degre ()
{
    QStringList retour;
    for (int i=14;i<18;++i)
        if (bits.testBit (i))
            retour << degres[i-14];
    return retour.join (" ");
}
*/

int Traits::gradus ()
{
    QStringList retour;
    for (int i=15;i<19;++i)
        if (bits.testBit (i))
            return i-15;
    return 0;
}


/*
QString Traits::personne ()
{
    QStringList retour;
    for (int i=18;i<22;++i)
        if (bits.testBit (i))
            retour << personnes[i-18];
    return retour.join (" ");
}
*/

int Traits::persona ()
{
    for (int i=19;i<23;++i)
        if (bits.testBit (i))
            return i-19;
    return 0;
}

/*
QString Traits::temps ()
{
    QStringList retour;
    for (int i=22;i<29;++i)
        if (bits.testBit (i))
            retour << tempss[i-22];
    return retour.join (" ");
}
*/

int Traits::tempus ()
{
    for (int i=23;i<30;++i)
        if (bits.testBit (i))
            return i-23;
    return 0;
}

/*
QString Traits::mode ()
{
    QStringList retour;
    for (int i=29;i<37;++i)
        if (bits.testBit (i))
            retour << modes[i-29];
    return retour.join (" ");
}
*/

int Traits::modus ()
{
    for (int i=30;i<39;++i) // Ajout du supin 37 -> 38
        if (bits.testBit (i))
            return i-30;
    return 0;
}

/*
QString Traits::voix ()
{
    QStringList retour;
    for (int i=37;i<39;++i)
        if (bits.testBit (i))
            retour << voixs[i-37];
    return retour.join (" ");
}
*/

int Traits::uox ()
{
    for (int i=39;i<42;++i) // Ajout du supin 37 -> 38 et 40 -> 41
        if (bits.testBit (i))
            return i-39; // Ajout du supin 37 -> 38
    return 0;
}

bool Traits::contient (Traits * t)
{
    return (bits & t->getBits ()) == t->getBits ();
}

