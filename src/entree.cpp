/*
 *                          entree.cpp
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


#include "entree.h"

/*
Entree::Entree(QObject *parent) :
    QObject(parent)
{
}
*/
/* classe des entrées */

// Entree::Entree (QString linea, QObject *parent)
Entree::Entree (QString linea, QObject *parent) :
    QObject(parent)
{
//    lexique = qobject_cast<Lexicum*>(parent);
    QString erreur;
    QStringList eclats = linea.split ('|');
    if (eclats.count () < 5)
    {
        erreur = "error in linea " + linea ;
    }
    else
    {
        QStringList ek = eclats[0].split ('=');
        kq = ote2 (ek.at (0), nh);
        if (ek.count () > 1) fcq = ek.at (1);
        else fcq = kq;
        indMorph = eclats[4];
        kjv = Ch::ote_diacritiques (kq);
        k = Ch::deramise (kq);
        mdl = eclats[1].toInt ();
        init_quantites ();
        // quelques propriétés
        if (eclats.count () == 5) avec_locatif = false;
        else avec_locatif = eclats[5].contains("loc.");
        neutre = init_neutre ();
        init_nombre ();
//        init_locatif ();
        deponent = init_deponent ();
        intrans = init_intrans ();
        init_radicaux (eclats);
        // renvoi vers une autre entrée
        QRegExp c ("cf\\.\\s\\w+$");
        int pos = c.indexIn (indMorph);
        if (pos > -1)
        {
            cf = Ch::deramise (indMorph.mid (pos + 4));
        }
        else cf = "";
        hyphen = "";
    }
#ifdef DEBOG
    if (!erreur.isEmpty ())
    {
        qDebug () << erreur;
    }
#endif
}

Entree::~Entree ()
{
    while (irregs.count () > 0)
    {
        delete irregs.first ();
        irregs.erase (irregs.begin ());
    }
//    foreach (QString t, trad)
  //      delete t;
    trad.clear ();
}

const QStringList Entree::modeles = QStringList ()
        << "uita" << "amicus" << "puer" << "ager" << "templum"        //  0 - 4
        << "miles" << "ciuis" << "corpus" << "mare" << "manus"        //  5 - 9
        << "res" << "bonus" << "miser" << "pulcher" << "fortis"       // 10 - 14
        << "uetus" << "acer" << "amo" << "moneo" << "lego"            // 15 - 19
        << "capio" << "audio" << "sum" << "eo" << "imitor"            // 20 - 24
        << "uereor" << "sequor" << "patior" << "potior"             // 25 - 28
        << "pr. et adj. indef."                                // 29
        << "invaria" << "adv -e" << "adv -er"  // 30-32
        << "fero" << "nullus" << "fio" << "facio" ;         // 33 - 36

const QStringList Entree::mdlGrecs = QStringList ()
        << "Aeneas" << "cometes" << "Antiope" << "Delos"
        << "Ilion" << "Perseus" << "Socrates" << "Hector"
        << "lexis, eos" << "Capys, yos" << "adj. -os, on";

/*
 * init_radicaux calcule les radicaux en fonction
 * du modèle du lemme, et les insère dans la liste.
 */
void Entree::init_radicaux (QStringList eclats)
{
    QString rdu = Ch::ini (fcq, 1);
    QString rdx = Ch::ini (fcq, 2);
    switch (mdl)
    {
        case 100: // Aeneas
        case 101: // cometes
        case 102: // Antiope
        case 103: // Delos
        case 104: // Ilion
        case 0: // uita
            if (!eclats[2].isEmpty ())
            {
                radq[0] << eclats[2].split (",");
            }
            else if (nombre == 2 || k.endsWith ("n") || k.endsWith ("s"))
                radq[0] << rdx;
            else radq[0] << rdu;
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"āe").arg (k), this);
            break;
        case 105: // Perseus
            radq[0] << Ch::ini (fcq,3)+"ĕ"; // e-bref pour le radical
            irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (rdu).arg (k), this);
            break;
        case 1: // dominus
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (nombre == 2)
                radq[0] << rdu;
            else radq[0] << rdx;
            if (neutre)
            {
                irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ŭs").arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|3|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ŭs").arg (k), this);
                nombre = 1;
            }
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ī").arg (k), this);
            break;
        case 2: // puer
            if (nombre == 2)
                radq[0] << rdu;
            else
            {
                radq[0] << fcq;
                if (nombre != 2)
                {
                    irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                    irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                }
            }
            break;
        case 3: // ager
            if (!eclats[2].isEmpty ())
            {
                radq[0] << eclats[2].split (","); // à corriger dans lemmata.la
            }
            else radq[0] << rdx+"r";
            if (nombre != 2)
            {
                irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 4: // templum
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (nombre==2)
                radq[0] << rdu;
            else radq[0] << rdx;
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ī").arg (k), this);
            break;
        case 106: // Socrates
        case 108: // lexis, eos
            if (eclats[2].isEmpty ())
                radq[0] << rdx;
//                radq[0] << fcq + " génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            break;
    case 109: // Capys, yos
        if (eclats[2].isEmpty ())
            radq[0] << rdx;
//                radq[0] << fcq + " génitif manquant";
        else
        {
            radq[0] << eclats[2].split (",");
        }
        if (!fcq.endsWith("ys") && !fcq.endsWith("ўs"))
            irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
        if (neutre)
        {
            irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1*|%2|3|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            nombre = 1;
        }
        break;
        case 107: // imparisyllabiques grecs
        case 5: // miles
            if (eclats[2].isEmpty ())
                radq[0] << fcq+" génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            if (nombre != 2)
            {
                irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ī").arg (k), this);
            break;
        case 6: // ciuis
            if (eclats[2].isEmpty ())
                radq[0] << rdx; //fcq + " génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            if ((nombre != 2 ) && !k.endsWith ("is"))
            {
                irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
                irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
            }
            else if (k != radq[0].at(0)+"is") // vis
            {
                irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
            }
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ī").arg (k), this);
            break;
        case 7: // corpus
            if (eclats[2].isEmpty ())
                radq[0] << fcq + " génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            if (nombre != 2)
            {
                irregs << new Irreg (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|3|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (radq[0].at(0)+"ī").arg (k), this);
            break;
        case 8: // mare
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else radq[0] << rdu;
            if ((nombre != 2) && !k.endsWith("e")) // pour animal et exemplar
            {
                irregs << new Irreg (QString ("%1*|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 9: // manus
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (neutre) // cornu
                radq[0] << rdu;
            else radq[0] << rdx;
            if (avec_locatif)
                irregs << new Irreg (QString ("%1|%2|7|0|1|0|0|0|0|0").arg (rdx+"ī").arg (k), this);
            break;
        case 10: // res
            radq[0] << rdx;
            break;
        case 11: // doctus
            if (nombre==2)
                radq[0] << rdu;
            else radq[0] << rdx;
            if (!eclats[2].isEmpty ())
            {
                if (eclats[2] != "-") radq[1] << eclats[2].split (",");
            }
            else radq[1] << rdx + "\u012d";

            if (!eclats[3].isEmpty ())
            {
                if (eclats[3] != "-") radq[2] << eclats[3].split (",");
            }
            else radq[2] << rdx + "\u012bss\u012dm";
            break;
    case 110: // adj. grec en -os, on
        radq[0] << rdx;
        break;
        case 12: // miser
            if (nombre==2)
                radq[0] << rdu;
            else
            {
                radq[0] << fcq;//rdx;
                irregs << new Irreg (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            if (!eclats[2].isEmpty ())
            {
                if (eclats[2] != "-") radq[1] << eclats[2].split (",");
            }
            else
            {
                radq[1] << fcq+"\u012d";
            }
            if (!eclats[3].isEmpty ())
            {
                radq[2] << eclats[3].split (",");
            }
            else radq[2] << rdx+"\u0113rr\u012dm";
            break;
        case 13: // pulcher
            irregs << new Irreg (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            if (!eclats[2].isEmpty ()) // radical de génitif en champ 3
            {
                radq[0] << eclats[2].split (",");
            }
            else radq[0] << rdx+"r";
            if (!eclats[3].isEmpty ()) // radical de superlatif (sûrement inutile)
            {
                if (eclats[3] != "-") radq[2] << eclats[3].split (",");
            }
            else  // radicaux comparatif et superlatif
            {
                radq[1] << radq[0].at(0)+"\u012d";
                radq[2] << rdx+"\u0113rr\u012dm";
            }
            break;
        case 14: // fortis et prudens
            // positif
            if (!eclats[2].isEmpty ())
            {
                radq[0] << eclats[2].split (",");
            }
            else
            {
                radq[0] << rdx;
            }
            // comparatifs
            for (int i = 0; i < radq[0].size(); i++)
            {
                radq[1] << radq[0].at(i)+"ĭ";
            }
            // superlatifs
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            else
            {
                for (int i = 0; i < radq[0].size(); i++)
                {
                    radq[2] << radq[0].at(i)+"īssĭm";
                }
            }
            // pseudo-irréguliers prudens, ferox
            if (!k.endsWith ("is"))
            {
                irregs << new Irreg (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this); // nms
                irregs << new Irreg (QString ("%1*|%2|1|2|1|0|0|0|0|0").arg (fcq).arg (k), this); // nfs
                irregs << new Irreg (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this); // vms
                irregs << new Irreg (QString ("%1*|%2|2|2|1|0|0|0|0|0").arg (fcq).arg (k), this); // vfs
                irregs << new Irreg (QString ("%1*|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // nns
                irregs << new Irreg (QString ("%1*|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // vns
                irregs << new Irreg (QString ("%1*|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // acns
                for (int i = 0; i < radq[0].size(); i++)
                {
                    QString prudente = radq[0].at(i) + "ĕ"; // abl. sing. altenatif
                    irregs << new Irreg (QString ("%1|%2|6|1|1|0|0|0|0|0").arg (prudente).arg (k), this); // abl. ms
                    irregs << new Irreg (QString ("%1|%2|6|2|1|0|0|0|0|0").arg (prudente).arg (k), this); // abl. fs
                }
            }
            break;
        case 15: // uetus, diues
            if (eclats[2].isEmpty ())
                radq[0] << fcq + " erreur, rad gen. absent";
            else
            {
                radq[0] << eclats[2].split (",");
                foreach (QString rqz, radq[0])
                {
                    radq[1] << rqz+"\u012d";
                    if (rqz.endsWith ("\u0115r"))  // uetĕr­ ueterrimus
                        radq[2] << rqz + "r\u012dm";
                    else radq[2] << rqz+"iss\u012dm";
                }
            }
            irregs << new Irreg (QString ("%1|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|1|2|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|2|2|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Irreg (QString ("%1|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            break;
        case 16: // acer
            {
                // tous degrés construits sur r1
                radq[0] << rdx + 'r';
                radq[1] << rdx+"r\u012d";
                radq[2] << fcq+"r\u012dm";
                irregs << new Irreg (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Irreg (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 17: // amo
            radq[0] << rdu;
            if (!eclats[2].isEmpty ())
                radq[1] << eclats[2].split (",");
            else radq[1] << rdu + "\u0101v"; // āv
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            else radq[2] << rdu + "\u0101t"; // āt
            // adj. verbal
            radq[3] << rdu + "\u0101"; // ā
            radq[4] << radq[3];
            break;
        case 18: // moneo
            radq[0] << rdx;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            radq[3] << rdx + "\u0113"; // ē
            radq[4] << radq[3];
            break;
        case 33:
            irregs << new Irreg (QString ("%1*|%2|0|0|1|0|2|1|3|1").arg (rdu).arg (k), this);
        case 19: // lego
            radq[0] << rdu;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            radq[3] << rdu + "\u0113"; // ē
            radq[4] << radq[3];
            radq[4] << rdu + "\u016b"; // ū
            break;
        case 36: // facio
            irregs << new Irreg (QString ("%1*|%2|0|0|1|0|2|1|3|1").arg (rdx).arg (k), this);
        case 20: // capio
        case 21: // audio
            radq[0] << rdx;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            radq[3] << rdu + "\u0113"; // ē
            radq[4] << radq[3];
            radq[4] << rdu + "\u016b"; // ū
            break;
        case 35: // fio presque comme audio
            radq[0] << rdx;
            // pas de parf. radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            // pas de part. prés. radq[3]
            radq[4] << rdx + "ăcĭē"; // faciendus
            break;
        case 22: // sum
            if (k.length () < 4)
                radq[0] << ".";
            else radq[0] << Ch::ini (fcq, 3);
            if (eclats[2].isEmpty ())
                radq[1] << Ch::ini (fcq, 3)+"f\u016d"; // fŭ
            else radq[1] << eclats[2].split (",");
            if (eclats[3].isEmpty ())
                radq[2] << radq[0];
            else radq[2] << eclats[3];
            break;
        case 23: // eo
            if (k.length () < 3)
                radq[0] << ".";
            else radq[0] << rdx;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            break;
        case 24: // imitor
            radq[0] << rdx;
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            else radq[2] << rdx + "\u0101t"; // "āt";
            radq[3] << rdx + "\u0101";
            radq[4] << radq[3];
            break;
        case 25 : // uereor
            radq[0] << Ch::ini (fcq, 3);
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            radq[3] << Ch::ini (fcq, 3) + "\u0113";
            radq[4] << radq[3];
            break;
        case 26: // sequor
            radq[0] << rdx;
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            radq[3] << rdx + "\u0113";
            radq[4] << radq[3];
            break;
        case 27: // patior
        case 28: // potior
            radq[0] << Ch::ini (fcq, 3);
            if (!eclats[3].isEmpty ())
                radq[2] << eclats[3].split (",");
            radq[3] << rdx + "\u0113";
            radq[4] << radq[3];
            break;
        case 29:
            {
                int nbre = 1;
                if (nombre == 2) nbre = 2;
                if (!kq.startsWith ("s\u0113")) irregs << new Irreg (QString ("%1|%2|1|1|%3|0|0|0|0|0").arg (fcq).arg (k).arg (nbre), this);
                break;
            }
        case 30:
        case 31:
        case 32:
            irregs << new Irreg (QString ("%1|%2|0|0|0|0|0|0|0|0").arg (fcq).arg (k), this);
            break;
    case 34:
        irregs << new Irreg (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
        if (!eclats[2].isEmpty ())
            radq[0] << eclats[2].split (",");
        break;
    }
}

QList<Irreg*> Entree::irreguliers ()
{
    return irregs;
}

void Entree::ajIrr (Irreg * irr)
{
    irregs << irr;
}

void Entree::init_quantites ()
{
    // fcq est la forme canonique entièrement mesurée.
    if (fcq.endsWith ("l"))  // brefs
    {
        fcq.replace (QRegExp ("al$"), "\u0103l");
        fcq.replace (QRegExp ("el$"), "\u0115l");
        fcq.replace (QRegExp ("il$"), "\u012dl");
        fcq.replace (QRegExp ("ol$"), "\u014fl");
        fcq.replace (QRegExp ("ul$"), "\u016dl");
    }
    else if (fcq.endsWith ("r"))  // brefs
    {
        fcq.replace (QRegExp ("ar$"), "\u0103r");
        fcq.replace (QRegExp ("er$"), "\u0115r");
        fcq.replace (QRegExp ("ir$"), "\u012dr");
        fcq.replace (QRegExp ("or$"), "\u014fr");
        fcq.replace (QRegExp ("ur$"), "\u016dr");
    }
    else if (fcq.endsWith ("m"))  // brefs
    {
        fcq.replace (QRegExp ("am$"), "\u0103m");
        fcq.replace (QRegExp ("em$"), "\u0115m");
        fcq.replace (QRegExp ("im$"), "\u012dm");
        fcq.replace (QRegExp ("om$"), "\u014fm");
        fcq.replace (QRegExp ("um$"), "\u016dm");
    }
    else if (((mdl>4)&&(mdl<8))||((mdl>13)&&(mdl<17)))
    {
        if (fcq.endsWith("is"))
            fcq.replace (QRegExp ("is$"), "\u012ds"); // ĭs bref
        else
        if (fcq.endsWith ("us"))
        {
            fcq.replace (QRegExp ("([^ēe])us$"), "\\1\u016ds"); // corpus
        }
        else if (fcq.endsWith ("o"))
        {
            fcq.replace (QRegExp ("o$"), "\u014d\u0306"); // o commun
        }
        else if (fcq.endsWith ("es"))
        {
            if (nombre==2) fcq.replace (QRegExp ("es$"), "\u0115s"); // e bref
            else fcq.replace (QRegExp ("es$"), "\u0113s");
        }
    }
}

QString Entree::canon ()
{
    return k;
}

QString Entree::canonR ()
{
    return kjv;
}

bool Entree::n_a_pas_trad ()
{
    return trad.isEmpty();
}

int Entree::numh ()
{
    return nh;
}

/*
 * Retourne la forme canonique équipée de
 * ses longues et brèves
 *
 * */
QString Entree::quantite ()
{
    return fcq;
}

int Entree::modele ()
{
    return mdl;
}

bool Entree::egale (Entree * e)
{
    if (e==NULL) return false;
    return (fcq == e->quantite ()
        && mdl == e->modele ()
        && nh == e->numh ());
}


char Entree::pars()
{
    if (mdl < 11) return 'n';
    if (mdl < 17) return 'a';
    if (mdl < 29 || mdl==33 || mdl==35 || mdl==36) return 'v'; // 33 == fero ; 35 == fio
    if (mdl == 29 || mdl == 34) return 'p';
    if (mdl > 50) return 'n';
    return 'i';
}

QStringList Entree::radicalq (int n)
{
    return radq [n-1];
}

QString Entree::grmodele ()
{
    if (mdl >99 && mdl < 111) return mdlGrecs[mdl-100];
    if (mdl < 0 || mdl > 36) return "modèle inconnu";
    return modeles [mdl];
}

/**
 * vrai si la définition inclut la chaîne ", .n ".
 */
bool Entree::init_neutre ()
{
    return indMorph.contains (", n.")
        || mdl == 4 || mdl == 7 || mdl == 8;
}

void Entree::init_nombre ()
{
    if ((mdl == 0 && k.endsWith ("ae"))
        || ((mdl>0 && mdl<4) && k.endsWith ("i"))
        || (mdl == 4 && k.endsWith ("a"))
        || (mdl == 11 && k.endsWith ("i"))
        || (indMorph.contains (" pl.")))
        nombre = 2;
    else if (indMorph.contains ("sing."))
        nombre = 1;
    else nombre = 3;
}
/*
void Entree::init_locatif()
{
    avec_locatif = indMorph.contains("+loc.");
    if (avec_locatif) indMorph.remove("+loc.");
}
*/
int Entree::getNombre ()
{
    return nombre;
}

bool Entree::aUnSingulier ()
{
    return nombre != 2;
}

bool Entree::aUnPluriel ()
{
    return nombre != 1;
}

bool Entree::init_deponent ()
{
    return (mdl > 23 && mdl < 29);
//    return ((mdl > 23 && mdl < 29) || k=="audeo");
}

bool Entree::init_intrans ()
{
    return indMorph.contains ("intr.");
}

// accesseurs des propriétés morpho
bool Entree::est_neutre ()
{
    return neutre;
}

bool Entree::est_deponent ()
{
    return deponent;
}

bool Entree::est_intransitif ()
{
    return intrans;
}

QString Entree::doc ()
{
    return indMorph;
}

void Entree::ajTrad (QString lang, QString tr)
{
    trad[lang] = tr;
}

QString Entree::traduction(QString lang)
{
    return trad[lang];
}

void Entree::setAdv (Entree * e)
{
    adv << e;
}

void Entree::setIndMorph (QString im)
{
    indMorph = im;
}

QList<Entree*> Entree::getAdv ()
{
    return adv;
}

QString Entree::ambrogio ()
{
    QString retour;
    QTextStream ss (&retour);
    ss << "<table>";
    foreach (QString lang, trad.keys ())
    {
        if (!trad[lang].isEmpty())
        ss << "<tr><td>- " << lang << "</td><td>&nbsp;" << trad[lang] << "</td></tr>\n";
    }
    ss << "</table>";
    return retour;
}

QString Entree::definition (QString l)
{
    QString retour = "<strong>%1</strong>, %2 : %3";
    retour = retour.arg(kq).arg(indMorph);
    if (!trad[l].isEmpty())
        return retour.arg (trad[l]);
    if (!trad["fr"].isEmpty())
        return retour.arg ("<i>en Français</i> : " + trad["fr"]);
    if (!trad["uk"].isEmpty())
        return retour.arg ("<i>in English</i> : " + trad["uk"]);
    return retour.arg ("<i>Non traduit</i>");
}

QString Entree::ote2 (QString k, int &h)
{
    if (k[k.length()-1].isDigit())
    {
        h = k[k.length()-1].digitValue();
        k.chop(1);
    }
    else h = 1;
    return k;
}

/* la même écrite par Yves
QString Entree::ote2 (QString k, int &h)
{
    bool ok;
    h = k.right(1).toInt (&ok);
    if (ok)
        k.chop (1);
    else h=1;
    return k;
}
*/

QString Entree::getCf ()
{
    return cf;
}

void Entree::setHyphen(QString h)
{
    hyphen = h;
}

QString Entree::getHyphen()
{
    return hyphen;
}

