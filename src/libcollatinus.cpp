/*
 *                           libcollatinus.cpp
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

/*
 *    *****************************
 *     garder l'encodage en UTF-8
 *    *****************************
 */

/*
 ********************  AGENDA ****************************
 *
 *  ADDENDA
 *  - ex(s)tirpo, are
 *
 */

#define DEBOG

#include <iostream>
#include <fstream>
#include <sstream>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QApplication>
#include <QMessageBox>
#include "libcollatinus.h"
// pour déboguer
#include <QDebug>
// pour l'affichage console
// #include <iostream>

using namespace std;

QString cass [7] =
    {"", "nom.", "uoc.", "acc.", "gen.", "dat.", "abl."};
QString genres [4] = {"", "masc.", "fem.", "neut."};
QString nombres [3] = { "", "sing.", "plur."};
QString personnes [4] = { "", "prim.", "sec.", "tert."};
QString degres [4] = { "", "posit.", "compar.", "superl."};
QString tempss [7] = { "", "praes.", "fut.", "imperf.", "perf.",
                      "fut. ant.", "plus-quam-perf."};
QString modes [8] =
{
    "", "ind.", "coniunct.", "imper.", "infin.", // 0 - 4
    "part.", "gerund.", "adiect. verb."
};             // 5 - 7

QString voixs [3] = { "", "act.", "pass."};

QString modeles [34] =
{
    "uita", "amicus", "puer", "ager", "templum",        //  0 - 4
    "miles", "ciuis", "corpus", "mare", "manus",        //  5 - 9
    "res", "bonus", "miser", "pulcher", "fortis",       // 10 - 14
    "uetus", "acer", "amo", "moneo", "lego",            // 15 - 19
    "capio", "audio", "sum", "eo", "imitor",            // 20 - 24
    "uereor", "sequor", "patior", "potior",             // 25 - 28
    "pr.et adj. indef.",                                // 29 
    "invaria","adv -e","adv -er", "fero"                // 30 - 33
};

QString mdlGrecs [10] =
{
    "Aeneas", "cometes", "Antiope", "Delos", "Ilion", "Perseus"
};

namespace Ch
{
    /**
     * ini  donne la chaine c tronquée de n caractères à droite
     *
     */
    QString ini (QString c, int n)
    {
        c.chop(n);
        return c;
    }

    /**
     * jviu transforme la graphie ramiste en graphie ancienne
     *
     */
    QString jviu (QString ch)
    {
        ch.replace ("J", "I");
        ch.replace ("j", "i");
        ch.replace ("v", "u");
        ch.replace ('V', 'U');
        return ch;
    }

/*
    QString stripAccents(const QString &original)
    {
        QString noAccents;
        // QString decomposed = original.normalized(QString::NormalizationForm_D);
        QString decomposed = original.normalized(QString::NormalizationForm_KD);
        foreach (const QChar &ch, decomposed) {
            if ( ch.category() != QChar::Mark_NonSpacing ) {
                noAccents.append(ch);
            }
        }
        return noAccents;
    }
*/
    void deQuant (QString *c)
    {
        // c->remove (0x0306); // combining
        if (c->endsWith("\u0306")) c->chop(1); // Supprimer le combining breve à la fin du mot
        c->replace (QRegExp ("[\u0101\u0103](m?)$"), "a\\1"); // ā ă
        c->replace (QRegExp ("[\u0113\u0115](m?)$"), "e\\1");
        c->replace (QRegExp ("[\u012b\u012d](m?)$"), "i\\1");
        c->replace (QRegExp ("[\u014d\u014f](m?)$"), "o\\1");
        c->replace (QRegExp ("[\u016b\u016d](m?)$"), "u\\1");
        c->replace (QRegExp ("[\u0232\u0233](m?)$"), "y\\1");
    }

    QString ote_diacritiques (QString k)
    {
        // minuscules
        k.replace (0x0101, 'a'); k.replace (0x0103, 'a');  // ā ă
        k.replace (0x0113, 'e'); k.replace (0x0115, 'e');  // ē ĕ
        k.replace (0x012b, 'i'); k.replace (0x012d, 'i');  // ī ĭ
        k.replace (0x014d, 'o'); k.replace (0x014f, 'o');  // ō ŏ
        k.replace (0x016b, 'u'); k.replace (0x016d, 'u');  // ū ŭ
        k.replace (0x0233, 'y'); k.replace (0x045e, 'y');  // ȳ ў
        // majuscule
        k.replace (0x0100, 'A'); k.replace (0x0102, 'A');  // Ā Ă
        k.replace (0x0112, 'E'); k.replace (0x0114, 'E');  // Ē Ĕ
        k.replace (0x012a, 'I'); k.replace (0x012c, 'I');  // Ī Ĭ
        k.replace (0x014c, 'O'); k.replace (0x014e, 'O');  // Ō Ŏ
        k.replace (0x016a, 'U'); k.replace (0x016c, 'U');  // Ū Ŭ
        k.replace (0x0232, 'Y'); k.replace (0x040e, 'Y');  // Ȳ Ў
        // combining breve
        k.remove (0x0306);
        return k;
    }

    QString deramise (QString k)
    {
        return jviu (ote_diacritiques (k));
    }

    QString versPedeCerto (QString k)
    {
        // Je remplace les longues par +, les brèves par - et les communes par *
        // minuscules
        k.replace (0x0101, '+'); k.replace (0x0103, '-');  // ā ă
        k.replace (0x0113, '+'); k.replace (0x0115, '-');  // ē ĕ
        k.replace (0x012b, '+'); k.replace (0x012d, '-');  // ī ĭ
        k.replace (0x014d, '+'); k.replace (0x014f, '-');  // ō ŏ
        k.replace (0x016b, '+'); k.replace (0x016d, '-');  // ū ŭ
        k.replace (0x0233, '+'); k.replace (0x045e, '-');  // ȳ ў
        // majuscule
        k.replace (0x0100, '+'); k.replace (0x0102, '-');  // Ā Ă
        k.replace (0x0112, '+'); k.replace (0x0114, '-');  // Ē Ĕ
        k.replace (0x012a, '+'); k.replace (0x012c, '-');  // Ī Ĭ
        k.replace (0x014c, '+'); k.replace (0x014e, '-');  // Ō Ŏ
        k.replace (0x016a, '+'); k.replace (0x016c, '-');  // Ū Ŭ
        k.replace (0x0232, '+'); k.replace (0x040e, '-');  // Ȳ Ў
        // "+" + combining breve = voyelle commune
        k.replace ("+\u0306", "*");
        k.replace ("[", "`");
        k.remove("]");
        // Je garde une trace de l'élision (pour le rythme)
        k.remove(reLettres);
        return k;
    }

    QString allonge (QString f)
    {
        int taille = f.size();
        // Je sais que le morceau à attacher commence par une consonne.
        if (Ch::consonnes.contains (f.at (taille-1))
            && !QString ("\u0101e \u0101u \u0113u \u014de").contains (f.mid (taille-3, 2).toLower()))
        {
            f.replace (QRegExp ("[a\u0103](["+Ch::consonnes+"])$"), "\u0101\\1");
            f.replace (QRegExp ("[e\u0115](["+Ch::consonnes+"])$"), "\u0113\\1");
            f.replace (QRegExp ("[i\u012d](["+Ch::consonnes+"])$"), "\u012b\\1");
            f.replace (QRegExp ("[o\u014F](["+Ch::consonnes+"])$"), "\u014d\\1");
            f.replace (QRegExp ("[u\u016d](["+Ch::consonnes+"])$"), "\u016b\\1");
            f.replace (QRegExp ("[y\u0233](["+Ch::consonnes+"])$"), "\u045e\\1");
            f.replace (QRegExp ("[A\u0102](["+Ch::consonnes+"])$"), "\u0100\\1");
            f.replace (QRegExp ("[E\u0114](["+Ch::consonnes+"])$"), "\u0112\\1");
            f.replace (QRegExp ("[I\u012c](["+Ch::consonnes+"])$"), "\u012a\\1");
            f.replace (QRegExp ("[O\u014e](["+Ch::consonnes+"])$"), "\u014c\\1");
            f.replace (QRegExp ("[U\u016c](["+Ch::consonnes+"])$"), "\u016a\\1");
            f.replace (QRegExp ("[Y\u0232](["+Ch::consonnes+"])$"), "\u040e\\1");
        }
        return f;
    }

    void allonge (QString * f)
    {
        *f = allonge (*f);
    }

    void elide (QString *mp)
    {
        int taille = mp->size ();
        if (taille > 1 && ((mp->endsWith ('m') || mp->endsWith ("\u0101e")) || mp->endsWith ("\u0306"))
            && voyelles.contains (mp->at (taille - 2)))
        {
            Ch::deQuant (mp);
            mp->insert (taille - 2, '[');
            mp->append (']');
        }
        else if (voyelles.contains (mp->at (taille - 1)))
        {
            Ch::deQuant (mp);
            mp->insert (taille - 1, '[');
            mp->append (']');
        }
    }


    QString assimile (QString f, int &i)
    {
        i = 0;
        QString p;
        foreach (p, assimAnte)
        {
            if (f.startsWith (p))
                return f.replace (QRegExp ("^"+p), assimPost[i]);
            i++;
        }
        i = 0;
        foreach (p, assimPost)
        {
            if (f.startsWith (p))
                return f.replace (QRegExp ("^"+p), assimAnte[i]);
            i++;
        }
        return f;
    }

    QString assimileq (QString f, int i)
    {
        if (f.startsWith (assimAnteq[i]))
            return f.replace (QRegExp ("^"+assimAnteq[i]), assimPostq[i]);
        return f.replace (QRegExp ("^"+assimPostq[i]), assimAnteq[i]);
    }

    bool estNombre (QString s)
    {
        bool ok=false;
        s.toInt (&ok);
        return ok;
    }
} // fin namespace Ch

/**
 * Fonction de conversion
 * des chiffres romains en chiffres arabes.
 *
 */
bool est_romain (QString R)
{
    return !(R.contains(QRegExp ("[^IVXLCDM]"))
             || R.contains("IL")
             || R.contains("IVI"));
}

int aRomano (QString R)
{
    if (R.size () == 0) return 0;
    // création de la table de conversion : pourrait être créée ailleurs.
    QMap<QChar,int> conversion;
    conversion['I']=1;
    conversion['V']=5;
    conversion['X']=10;
    conversion['L']=50;
    conversion['C']=100;
    conversion['D']=500;
    conversion['M']=1000;
    // calcul du résultat : ajout d'un terme si l'ordre est normal, soustraction sinon.
    int res=0;
    int conv_c;
    int conv_s = conversion[R[0]];
    for (int i = 0; i < R.count()-1; i++)
    {
        conv_c = conv_s;
        conv_s = conversion[R[i+1]];
        if (conv_c < conv_s)
            res -= conv_c;
        else res += conv_c;
    }
    res += conv_s;
    return res;
}

Traits::Traits (int k, int g, int n, int d, int p, int t, int m, int v)
{
    bits.resize (40);
    bits.setBit (k);
    bits.setBit (g+7);
    bits.setBit (n+11);
    bits.setBit (d+14);
    bits.setBit (p+18);
    bits.setBit (t+22);
    bits.setBit (m+29);
    bits.setBit (v+37);
}

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
    for (int i=0;i<40;++i)
    {
        if (bits.testBit (i))
        {
                 if (i < 7)  retour << cass[i];
            else if (i < 11) retour << genres[i-7];
            else if (i < 14) retour << nombres[i-11];
            else if (i < 18) retour << degres[i-14];
            else if (i < 22) retour << personnes[i-18];
            else if (i < 29) retour << tempss[i-22];
            else if (i < 37) retour << modes[i-29];
            else  retour << voixs[i-37];
        }
    }
    return retour.join (" ").simplified ();
}

// retourne le premier cas trouvé
int Traits::casus ()
{
    for (int i=0;i<7;++i)
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
    for (int i=7;i<11;++i)
        if (bits.testBit (i))
            return i-7;
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
    for (int i=11;i<14;++i)
        if (bits.testBit (i))
            return i-11;
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
    for (int i=14;i<18;++i)
        if (bits.testBit (i))
            return i-14;
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
    for (int i=18;i<22;++i)
        if (bits.testBit (i))
            return i-18;
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
    for (int i=22;i<29;++i)
        if (bits.testBit (i))
            return i-22;
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
    for (int i=29;i<37;++i)
        if (bits.testBit (i))
            return i-29;
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
    for (int i=37;i<40;++i)
        if (bits.testBit (i))
            return i-37;
    return 0;
}

bool Traits::contient (Traits * t)
{
    return (bits & t->getBits ()) == t->getBits ();
}


/*********************
 *                  *
 *    DESINENCES    *
 *                  *
 ********************/

Tdes::Tdes (QString linea)
{
    //gr|c|g|n|d|p|t|m|v|mo|rn|grq
    // 0 1 2 3 4 5 6 7 8 9  10 11
    //or|0|0|1|0|1|1|1|2|17|1|ŏr
    QStringList eclats = linea.split('|');
    gr = eclats[0];
    foreach (QString em, eclats[9].split (','))
        mo << em.toInt ();
    rn = eclats[10].toInt ();
    if (eclats.count () > 11)
    {
        grq = eclats[11];
    }
    else
    {
        grq = "";
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

QString Tdes::graphie ()
{
    return gr;
}

QString Tdes::grquant ()
{
    return grq;
}

QList <int> Tdes::mdl ()
{
    return mo;
}

QStringList Tdes::modele ()
{
    QStringList ret;
    foreach (int m, mo)
        ret << modeles[m];
    return ret;
}

int Tdes::nr ()
{
    return rn;
}

QString Tdes::morpho ()
{
    return traits->humain ();
};

Traits * Tdes::getTraits ()
{
    return traits;
}

/********************/
/*   AnalyseMorpho  */
/********************/

AnalyseMorpho::AnalyseMorpho (QString rq, Tdes * d, Tentree * en,
                              QString suff, bool ii)
{
    des = d;
    e = en;
    q = rq;
    dq = des->grquant ();
    suffixe = suff;
    db_i = ii;
    traits = d->getTraits ();
}

AnalyseMorpho::AnalyseMorpho (QString rq, Traits * t, Tentree * en, QString ddq)
{
    des = NULL;
    e = en;
    q = rq;
    dq = ddq;
    suffixe ="";
    db_i = false;
    traits = t;
}

AnalyseMorpho::~AnalyseMorpho ()
{
    delete traits;
}

void AnalyseMorpho::setUpper ()
{
    q[0] = q[0].toUpper ();
}

void AnalyseMorpho::assimileq (int i)
{
    q = Ch::assimileq (q, i);
}

void AnalyseMorpho::setSuff (QString s)
{
    suffixe = s;
}

QString AnalyseMorpho::getD ()
{
    return dq;
}

void AnalyseMorpho::setQ (QString nq)
{
    q = nq;
}

QString AnalyseMorpho::getQ ()
{
    return q;
}

QString AnalyseMorpho::getForm ()
{
    QString fq = q;
    if (dq.isEmpty ()) // Vient de la liste des irréguliers
    {
        if (suffixe.isEmpty ()) return fq;
        // les suffixes possibles sont que, ne et ve :
        // tous commencent par une consonne
        return Ch::allonge (fq) + suffixe;
    }
    if (fq == ".") // Radical vide de sum ou eo
    {
        if (suffixe.isEmpty ()) return dq;
        return Ch::allonge (dq) + suffixe;
    }
    // essai de correction pour do :
    if (!e->radicalq (2).isEmpty () && e->radicalq (2)[0].endsWith
        ("d\u0115d") && dq != "\u0101s")
        dq.replace (QRegExp ("^\u0101"), "\u0103");
    if (db_i) dq[0] = 0x012b; // ii -> 'ī';
    // -o des verbes
    if ((QString::compare (dq, "\u014d\u0306") == 0)
        && (QRegExp
            (".*(["+Ch::longues+"]["+Ch::consonnes+"]?|["+Ch::consonnes+"]{2}|[xj])$").exactMatch (q)))
        dq = "\u014d"; // o long

    // collage intelligent fq = fq + dq
    if (Ch::consonnes.contains(dq[0]))
        fq = Ch::allonge (fq) + dq;
    else
    {
        // fq est un radical prêt à accepter une
        // désinence commençant par une voyelle : le
        // cas de praeest doit passer dans case 22
        fq = fq + dq;
    }
    // puis le suffixe.
    if (suffixe.isEmpty ()) return fq;
    return Ch::allonge (fq) + suffixe;
}

Tentree * AnalyseMorpho::entree ()
{
    return e;
}

Traits * AnalyseMorpho::getTraits ()
{
    return traits;
}

QString AnalyseMorpho::humain ()
{
    return traits->humain ();
}

/********************/
/* Les irréguliers  */
/********************/

Tirr::Tirr (QString linea, Tentree * entr)
{
    // maximis|magnus|5|1|2|0|0|0|0|0
    // gr        k    c g n d p t m v
    QStringList eclats = linea.split ('|');
    if (eclats.count () != 10)
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
}

Tirr::~Tirr ()
{
   delete traits;
}

Tentree * Tirr::entree ()
{
    return e;
}

QString Tirr::graphie ()
{
    return gr;
}

QString Tirr::graphieq ()
{
    return grq;
}

QString Tirr::morpho ()
{
    return traits->humain ();
}

Traits * Tirr::getTraits ()
{
    return traits;
}

bool Tirr::exclusif ()
{
    return excl;
}

bool Tirr::homomorphe (Tdes * des)
{
    return traits->getBits () == des->getTraits ()->getBits ();
}

/* classe des radicaux */

Tradix::Tradix (Tentree *entree, int m, int n, QString q, bool ii)
{
    e = entree;
    k = entree->canon ();
    mo = m;
    num = n;
    eni = ii;
    grq = q;
}

void Tradix::ajouteEntree (Tentree *entree)
{
    e = entree;
}

int Tradix::mdl ()
{
    return mo;
}

bool Tradix::en_i ()
{
    return eni;
}

int Tradix::nr ()
{
    return num;
}

Tentree * Tradix::entree ()
{
    return e;
}

QString Tradix::canon ()
{
    return k;
}

QString Tradix::grquant ()
{
    return grq;
}

QString Tradix::doc ()
{
    QString sortie;
    QTextStream (&sortie)
        << "rad. " << num << " de " << " (" << modeles[mo] << ")";
    return sortie;
}

/* classe des entrées */

QString Tentree::ote2 (QString k, int &h)
{
    if (k.endsWith ("2"))
    {
        h = 2;
        k.chop (1);
    }
    else if (k.endsWith ("3"))
    {
        h = 3;
        k.chop (1);
    }
    else h = 1;
    return k;
}

QString Tentree::getCf ()
{
    return cf;
}

Tentree::Tentree (QString linea)
{
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
        neutre = init_neutre ();
        init_nombre ();
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
    }
#ifdef DEBOG
    if (!erreur.isEmpty ())
    {
        qDebug () << erreur;
    }
#endif
}

Tentree::~Tentree ()
{
    while (irregs.count () > 0)
    {
        delete irregs.first ();
        irregs.erase (irregs.begin ());
    }
    foreach (QString * t, trad)
        delete t;
    trad.clear ();
}

/*
 * init_radicaux calcule les radicaux en fonction
 * du modèle du lemme, et les insère dans la liste.
 */
void Tentree::init_radicaux (QStringList eclats)
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
            break;
        case 105: // Perseus
            radq[0] << Ch::ini (fcq,3)+"ĕ"; // e-bref pour le radical
            irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (rdu).arg (k), this);
            break;
        case 1: // dominus
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (nombre == 2)
                radq[0] << rdu;
            else radq[0] << rdx;
            break;
        case 2: // puer
            if (nombre == 2)
                radq[0] << rdu;
            else
            {
                radq[0] << fcq;
                if (nombre != 2)
                {
                    irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                    irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
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
                irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 4: // templum
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (nombre==2)
                radq[0] << rdu;
            else radq[0] << rdx;
            break;
        case 5: // miles
            if (eclats[2].isEmpty ())
                radq[0] << fcq+" génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            if (nombre != 2)
            {
                irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 6: // ciuis
            if (eclats[2].isEmpty ())
                radq[0] << fcq + " génitif manquant";
            else
            {
                radq[0] << eclats[2].split (",");
            }
            if ((nombre != 2 ) && !k.endsWith ("is"))
            {
                irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
                irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
            }
            else if (k != radq[0].at(0)+"is") // vis
            {
                irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this); // exclusif
            }
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
                irregs << new Tirr (QString ("%1*|%2|1|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|3|0|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 8: // mare
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else radq[0] << rdu;
            if ((nombre != 2) && !k.endsWith("e"))
            {
                irregs << new Tirr (QString ("%1*|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            }
            break;
        case 9: // manus
            if (!eclats[2].isEmpty ())
                radq[0] << eclats[2].split (",");
            else if (neutre) // cornu
                radq[0] << rdu;
            else radq[0] << rdx;
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
        case 12: // miser
            if (nombre==2)
                radq[0] << rdu;
            else
            {
                radq[0] << fcq;//rdx;
                irregs << new Tirr (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
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
            irregs << new Tirr (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
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
                irregs << new Tirr (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this); // nms
                irregs << new Tirr (QString ("%1*|%2|1|2|1|0|0|0|0|0").arg (fcq).arg (k), this); // nfs
                irregs << new Tirr (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this); // vms
                irregs << new Tirr (QString ("%1*|%2|2|2|1|0|0|0|0|0").arg (fcq).arg (k), this); // vfs
                irregs << new Tirr (QString ("%1*|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // nns
                irregs << new Tirr (QString ("%1*|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // vns
                irregs << new Tirr (QString ("%1*|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this); // acns
                for (int i = 0; i < radq[0].size(); i++)
                {
                    QString prudente = radq[0].at(i) + "ĕ"; // abl. sing. altenatif
                    irregs << new Tirr (QString ("%1|%2|6|1|1|0|0|0|0|0").arg (prudente).arg (k), this); // abl. ms
                    irregs << new Tirr (QString ("%1|%2|6|2|1|0|0|0|0|0").arg (prudente).arg (k), this); // abl. fs
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
            irregs << new Tirr (QString ("%1|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|1|2|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|2|2|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|1|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|2|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            irregs << new Tirr (QString ("%1|%2|3|3|1|0|0|0|0|0").arg (fcq).arg (k), this);
            break;
        case 16: // acer
            {
                // tous degrés construits sur r1
                radq[0] << rdx + 'r';
                radq[1] << rdx+"r\u012d";
                radq[2] << fcq+"r\u012dm";
                irregs << new Tirr (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
                irregs << new Tirr (QString ("%1*|%2|2|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
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
            irregs << new Tirr (QString ("%1*|%2|0|0|1|0|2|1|3|1").arg (rdu).arg (k), this);
        case 19: // lego
            radq[0] << rdu;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            radq[3] << rdu + "\u0113"; // ă
            radq[4] << radq[3];
            radq[4] << rdu + "\u016b"; // ū
            break;
        case 20: // capio
        case 21: // audio
            radq[0] << rdx;
            radq[1] << eclats[2].split (",");
            radq[2] << eclats[3].split (",");
            radq[3] << rdu + "\u0113"; // ă
            radq[4] << radq[3];
            radq[4] << rdu + "\u016b"; // ū
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
                if (!kq.startsWith ("s\u0113")) irregs << new Tirr (QString ("%1|%2|1|1|%3|0|0|0|0|0").arg (fcq).arg (k).arg (nbre), this);
                break;
            }
        case 30:
        case 31:
        case 32:
            irregs << new Tirr (QString ("%1|%2|0|0|0|0|0|0|0|0").arg (fcq).arg (k), this);
            break;
    case 34:
        irregs << new Tirr (QString ("%1*|%2|1|1|1|0|0|0|0|0").arg (fcq).arg (k), this);
        if (!eclats[2].isEmpty ())
            radq[0] << eclats[2].split (",");
        break;
    }
}

QList<Tirr*> Tentree::irreguliers ()
{
    return irregs;
}

void Tentree::ajIrr (Tirr * irr)
{
    irregs << irr;
}

void Tentree::init_quantites ()
{
    // fcq = kq; // fcq est la forme canonique entièrement mesurée.
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

QString Tentree::canon ()
{
    return k;
}

QString Tentree::canonR ()
{
    return kjv;
}

int Tentree::numh ()
{
    return nh;
}

/*
 * Retourne la forme canonique équipée de
 * ses longues et brèves
 *
 * */
QString Tentree::quantite ()
{
    return fcq;
}

int Tentree::modele ()
{
    return mdl;
}

bool Tentree::egale (Tentree * e)
{
    if (e==NULL) return false;
    return (fcq == e->quantite ()
        && mdl == e->modele ()
        && nh == e->numh ());
}


char Tentree::pars ()
{
    if (mdl < 11) return 'n';
    if (mdl < 17) return 'a';
    if (mdl < 29 || mdl==33) return 'v'; // 33 == fero
    if (mdl == 29 || mdl == 34) return 'p';
    if (mdl > 50) return 'n';
    return 'i';
}

QStringList Tentree::radicalq (int n)
{
    return radq [n-1];
}

QString Tentree::grmodele ()
{
    if (mdl >99 && mdl < 110) return mdlGrecs[mdl-100];
    if (mdl < 0 || mdl > 33) return "modèle inconnu";
    return modeles [mdl];
}

/**
 * vrai si la définition inclut la chaîne ", .n ".
 */
bool Tentree::init_neutre ()
{
    return indMorph.contains (", n.")
        || mdl == 4 || mdl == 7 || mdl == 8;
}

void Tentree::init_nombre ()
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

int Tentree::getNombre ()
{
    return nombre;
}

bool Tentree::aUnSingulier ()
{
    return nombre != 2;
}

bool Tentree::aUnPluriel ()
{
    return nombre != 1;
}

bool Tentree::init_deponent ()
{
    return (mdl > 23 && mdl < 29);
}

bool Tentree::init_intrans ()
{
    return indMorph.contains ("intr.");
}

// accesseurs des propriétés morpho
bool Tentree::est_neutre ()
{
    return neutre;
}

bool Tentree::est_deponent ()
{
    return deponent;
}

bool Tentree::est_intransitif ()
{
    return intrans;
}

QString Tentree::doc ()
{
    return indMorph;
}

void Tentree::ajTrad (QString lang, QString * tr)
{
    trad[lang] = tr;
}

QString * Tentree::traduction (QString lang)
{
    return trad[lang];
}

void Tentree::setAdv (Tentree * e)
{
    adv << e;
}

void Tentree::setIndMorph (QString im)
{
    indMorph = im;
}

QList<Tentree*> Tentree::getAdv ()
{
    return adv;
}

QString Tentree::ambrogio ()
{
    QString retour;
    QTextStream ss (&retour);
    ss << "<table>";
    foreach (QString lang, trad.keys ())
    {
        if (trad[lang] != NULL)
        ss << "<tr><td>- " << lang << "</td><td>&nbsp;" << *trad[lang] << "</td></tr>\n";
    }
    ss << "</table>";
    return retour;
}

QString Tentree::definition (QString l)
{
    if (trad[l] != NULL)
        return QString ("<strong>%1</strong>, %2 : %3")
            .arg (kq)
            .arg (indMorph)
            .arg (*trad[l]);
    if (trad["fr"] != NULL)
        return QString ("<strong>%1</strong>, %2 : %3")
            .arg (kq)
            .arg (indMorph)
                .arg ("<i>en Français</i> : " + *trad["fr"]);
    if (trad["uk"] != NULL)
        return QString ("<strong>%1</strong>, %2 : %3")
            .arg (kq)
            .arg (indMorph)
                .arg ("<i>in English</i> : " + *trad["uk"]);
        return QString ("<strong>%1</strong>, %2 : %3")
            .arg (kq)
            .arg (indMorph)
            .arg ("<i>Non traduit</i>");
}

/*******************
** classe lexique **
********************/

TLexicum::TLexicum ()
{
    // lecture du fichier des modèles de radicaux mdlrad.la
    QString fichier = qApp->applicationDirPath () + "/ressources/mdlrad.la";
    QFile fmr (fichier);
    fmr.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fluxM (&fmr);
    QString linea;
    while (!fluxM.atEnd ())
    {
        linea = fluxM.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        QStringList eclats = linea.split ('|');
        int m = eclats[0].toInt ();
        int c = eclats.count ();
        for (int i=1;i<c;++i)
        {
            modelesRad[m] << eclats[i].toInt ();
        }
    }
    fmr.close ();

    // Lecture du fichier de lemmes lemmata.la
    lemmataLege ();

    // fichier de désinences desin.la
    fichier = qApp->applicationDirPath () + "/ressources/desin.la";
    QFile fDes (fichier);
    fDes.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fluxD (&fDes);
    fluxD.setCodec("UTF-8");
    while (!fluxD.atEnd ())
    {
        linea = fluxD.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        Tdes D = Tdes (linea);
        desinentiae.insert (c2d::value_type (D.graphie (), D));
        fluxD.flush ();
    }
    fDes.close ();

    // fichier des formes irrégulières irregs.la
    fichier = qApp->applicationDirPath () + "/ressources/irregs.la";
    QFile fIrr (fichier);
    fIrr.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fluxI (&fIrr);
    fluxI.setCodec("UTF-8");
    while (!fluxI.atEnd ())
    {
        linea = fluxI.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        QString cle = linea.section ('|', 1, 1);
        Tentree * e = entree (cle);
        Tirr * I = new Tirr (linea, e);
        if (e != NULL) e->ajIrr (I);
#ifdef DEBOG
        else qDebug () << "erreur irr." << linea;
#endif
        irregulares.insert (c2i::value_type (Ch::deramise (I->graphie ()), I));
        fluxI.flush ();
    }
    fIrr.close ();

    // initialisation des morphos d'adverbes
    //    Traits (int k, int g, int n, int d, int p, int t, int m, int v);
    bitsComp = new Traits (1,3,1,2,0,0,0,0); // morpho de l'adj = adv au comparatif
    bitsSup = new Traits (2,1,1,3,0,0,0,0);  // au superlatif

    // lecture du fichier des pronoms composés pronomina.la
    pronominaLege ();

    // lecture des fichiers de traduction
    QDir repertoire = QDir (qApp->applicationDirPath () +"/ressources/", "lemmata.*");
    QStringList lemmata = repertoire.entryList ();
    lemmata.removeOne ("lemmata.la");
    foreach (QString lem, lemmata)
    {
        QString ext = QFileInfo (lem).suffix ();
        linguamLege (ext);
        // traduction des entrées à renvoi 'cf.'
        foreach (Tentree * el, entreesCf)
        {
            if (el == NULL)
            {
                continue;
            }
            Tentree * ebis = renvoi (el);
#ifdef DEBOG
            if (ebis == NULL) qDebug () << el->quantite () << "renvoi nul";
#endif
            if (ebis != NULL && ebis != el)
            {
                el->ajTrad (ext, ebis->traduction (ext));
            }
        }
    }
    entreesCf.clear ();

    // ajout du pointeur sur adverbe
    while (!adjAdv.isEmpty ())
    {
        Tentree * el = adjAdv.takeFirst ();
        if (el != NULL)
        {
            foreach (QString a, el->doc().section ("@", -1).trimmed ().split (','))
            {
#ifdef DEBOG
                if (entrees.count (a) == 0) qDebug () << el->canon () << el->doc () << "adv. entrée nulle";
                else
#endif
                    el->setAdv (entrees[a]);
            }
            // supprimer @.*$ inutile
            el->setIndMorph (el->doc ().section ('@', 0, 0));
        }
    }

    // règles de quantité
    QFile fregles (qApp->applicationDirPath () +"/ressources/regles.txt");
    fregles.open (QFile::ReadOnly);
    QTextStream fle (&fregles);
    fle.setCodec ("UTF-8");
    QString ligne;
    QStringList rr;
    while (!fle.atEnd ())
    {
        ligne = fle.readLine ();
        if (!ligne.isEmpty () && ligne[0] != '!')
        {
            rr = ligne.split (";");
            regles.append (Regle (QRegExp (rr.at (0)), rr.at (1)));
        }
    }
    fregles.close ();
    voyelles = "a\u0101\u0103e\u0113\u0115i\u012b\u012do\u014d\u014fu\u016b\u016d";
    // ponctPhr = "[\\.\\;\\:\\?\\!]";
#ifdef DEBOG
    /*
    // DEBOG - CODE DE CHANGEMENT DE FORMAT
    // lemmata avec radicaux passés dans par_position
    QFile fLem ("ressources/lemmata.la");
    QTextStream flLem (&fLem);
    fLem.open (QIODevice::ReadOnly|QIODevice::Text);
    QFile fNouv ("lemmata.la.nouveau");
    fNouv.open (QIODevice::WriteOnly|QIODevice::Text);
    QTextStream flNouv (&fNouv);
    int h;
    while (!flLem.atEnd ())
    {
        QString l = flLem.readLine ().trimmed ();
        if (l.isEmpty () || l[0] == '!')
        {
            flNouv << l+"\n";
            continue;
        }
        QStringList ecl = l.split ("|");
        Tentree * e = entrees[Ch::deramise (ecl[0])];
        if (e == NULL)
        {
            qDebug () << l << "entrée nulle";
            continue;
        }
        QString cle = ecl[0];
        QString cleq = par_position (e->quantite ());
        QString cleqq = Tentree::ote2 (cle, h);
        if (cleqq != cleq)
            ecl[0] = cle + "=" + par_position (e->quantite ());
        else ecl[0] = cle;
        for (int n=2;n<4;++n)
        {
            if (ecl[n].isEmpty ()) continue;
            QStringList evirg = ecl[n].split (',');
            for (int i=0;i<evirg.count();++i)
                evirg[i]=par_position(evirg[i]);
            ecl[n] = evirg.join (",");
        }
        l = ecl.join ("|");
        flNouv << l+"\n";
    }
    fNouv.close ();
    fLem.close ();
    // FIN DEBOG
    */

    /*
    // doublons d'assimilation
    QFile fAssim ("assim-doublons.txt");
    fAssim.open (QIODevice::WriteOnly|QIODevice::Text);
    QTextStream fluxA (&fAssim);
    foreach (QString cle,  entrees.keys ())
    {
        Tentree * e = entrees[cle];
        if (e == NULL) continue;
        QString ass = assimile (e->canon ());
        if (ass == e->canon ()) continue;
        if (!entrees.keys ().contains (ass)) continue;
        if (entrees[ass] == NULL) continue;
        fluxA << QString ("%1\t%2\n").arg (e->quantite ()).arg (entrees[ass]->quantite ());
    }
    fAssim.close ();
    */
    /*
    // nettoyage des entrées nulles
    for (QHash<QString,Tentree*>::iterator it = entrees.begin ();it!=entrees.end ();it++)
    {

        if (it.value () == NULL)
        {
            qDebug () << it.key () << "entrée nulle";
            entrees.erase (it);
        }
        }
        */
#endif
}

TLexicum::~TLexicum ()
{
    /*
    while (!entrees.empty ())
    {
       delete entrees.begin ();
       entrees.erase (entrees.begin ());
    }
    delete  bitsComp;
    delete  bitsSup;
    while (radices.size () > 0)
        radices.erase (radices.begin ());
    */
}

QMap<QString,QString> TLexicum::cible ()
{
    return metae;
}

void TLexicum::lemmataLege ()
{
    // lit un lemmata.[lang]
    QString fichier = qApp->applicationDirPath () +"/ressources/lemmata.la";
    QFile Capsa (fichier);
    Capsa.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (&Capsa);
    flux.setCodec("UTF-8");
    QString linea;
    while (!flux.atEnd ())
    {
        linea = flux.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        QString cle = linea.section ("|",0,0).section('=',0,0);
        cle = Ch::deramise (cle);
        Tentree *E = new Tentree (linea);
        // ajout de l'entrée
        entrees[cle] = E;
        if (linea.contains ("@"))
        {
            adjAdv << E;
        }
        if (linea.contains ("cf. ")) entreesCf << E;
        // ajout des irréguliers canoniques
        foreach (Tirr * I, E->irreguliers ())
        {
            irregulares.insert (c2i::value_type (Ch::deramise (I->graphie ()), I));
        }
        // ajout des radicaux
        for (int n=1;n<=modelesRad[E->modele ()].count ();++n)
        {
            for (int i = 0; i < E->radicalq (n).size(); ++i)
            {
                QString rq = E->radicalq (n)[i];
                QString r = Ch::deramise (rq);
                if (!r.isEmpty ())
                {
                    // passage de Tentree::modeleRad à TLexicum::modelesRad[]
                    radices.insert (c2r::value_type (r, Tradix (E, modelesRad[E->modele ()][n-1], n, rq)));
                    // éviter les radicaux de comparatif et superlatif
                    if (rq.endsWith ("ĭ") && (modelesRad[E->modele ()][n]!=11 || n==1))
                    {
                        r.chop (1);
                        rq.chop (1);
                        radices.insert (c2r::value_type (r, Tradix (E, modelesRad[E->modele ()][n-1], n, rq, true)));
                    }
                }
            }
        }
        flux.flush();
    }
    Capsa.close ();
}

/*
 * pronominaLege () 
 * lit dans le fichier ressources/pronomina.la
 * les pronoms composés d'un pré- ou suffixe et d'un
 * pronom simple, qui peut se fléchir, dans se dernier
 * cas, à l'intérieur de la forme : quidam, gén.s. cuiusdam
 */
void TLexicum::pronominaLege ()
{
    QFile f (qApp->applicationDirPath () +"/ressources/pronomina.la");
    f.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (&f);
    flux.setCodec("UTF-8");
    QString genl = QString ("%1|%2|%3|%4|%5|0|0|0|0|0");
    QString linea;
    while (!flux.atEnd ())
    {
        linea = flux.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        // chercher le composé dans entrees
        QString canon = linea.section ('|', 0, 0);
        Tentree * eComp = entree (canon);
        if (eComp == NULL) continue;
        // chercher le simple dans entrees
        QString simple = linea.section ('|', 1, 1);
        Tentree * eSimple = entree (simple);
        if (eSimple == NULL) continue;
        QString affixe = linea.section ('|', 2, 2);
        bool dup = (affixe == "**");
        bool suff = (!dup) && affixe.startsWith ('*');
        affixe.remove ('*');
        // liste des irréguliers excl. de eComp.
        QList<int> irrList;
        foreach (Tirr * ii, eComp->irreguliers ())
            if (ii->exclusif ())
            {
                irrList << 6 * ii->getTraits ()->casus () 
                    + 2 * ii->getTraits ()->genus ()
                    + ii->getTraits ()->numerus ();
            }
        for (int c=1;c<7;++c)
            for (int g=1;g<4;++g)
                for (int n=1;n<3;++n)
                {
                    // abandon en cas de fc ou de présence d'un irrég exclusif
                    if ((c==1 && g==1 && n==1) || irrList.contains (6*c + 2*g + n))
                        continue;
                    QStringList fSimple = synthP (eSimple, c, n, g);
                    if (fSimple.empty ()) continue ;
                    foreach (QString fs, fSimple)
                    {
                        if (dup) fs = Ch::allonge (fs) + fs;
                        else if (suff) fs = Ch::allonge (fs) + affixe;
                        else fs = affixe + fs;
                        QString l = genl.arg (fs).arg (canon).arg (c).arg(g).arg(n); 
                        Tirr * irr = new Tirr (l, eComp);
                        eComp->ajIrr (irr);
                        irregulares.insert (c2i::value_type (Ch::deramise (irr->graphie ()), irr));
                    }
                }
    }
}

void TLexicum::linguamLege (QString lang)
{
    QString fichier = qApp->applicationDirPath () +"/ressources/lemmata."+lang;
    QFile Capsa (fichier);
    Capsa.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (&Capsa);
    flux.setCodec("UTF-8");
    QString linea;
    linea = flux.readLine ();
    // lire le nom de la langue
    linguaTr = flux.readLine ();
    linguaTr = linguaTr.mid (1).trimmed ();
    while (!flux.atEnd ())
    {
        linea = flux.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        QStringList ecl = linea.split ('|');
        if (ecl.count () < 2)
        {
#ifdef DEBOG
            qDebug () << "ligne mal formée" << linea;
#endif
            continue;
        }
        Tentree * e = entree (Ch::jviu (ecl[0]));
        if (e!=NULL)
            e->ajTrad (lang, new QString (ecl[1]));
#ifdef DEBOG
        else qDebug () << linea << "erreur linguamLege" << lang;
#endif
    }
    Capsa.close ();
    metae[lang] = linguaTr;
}

void TLexicum::dicLinguam (QString l)
{
    lingua = l;
}

void TLexicum::changeMajPert (bool m)
{
    MajPert = m;
}

QString TLexicum::lang ()
{
    return lingua;
}

/*
 * Ambrogio () traduit le mot latin dans
 * toutes les langues cibles disponibles
 */
QString TLexicum::ambrogio (Tentree * e)
{
    return e->ambrogio ();
}

/**
 * Renvoi d'entrée.
 * Détecte si l'entrée pointe sur une autre entrée,
 * et dans ce cas renvoie cette autre entrée.
 * sinon, renvoie la même entrée
 */
Tentree *TLexicum::renvoi (Tentree *e)
{
    QString r = e->getCf ();
    if (r.isEmpty ()) return e;
    return entree (r);
}

Tentree * TLexicum::entree (QString k)
{
#ifdef DEBOG
    if (entrees.count (k) == 0)
    {
        qDebug () << k << "entrée inexistante.";
        return NULL;
    }
    return entrees[k];
#else
    Tentree * e = entrees[k];
    if (e == NULL)
    {
        return NULL;
    }
    return e;
#endif
}

QString TLexicum::radical (QString r)
{
    c2r::iterator ir;
    QString sortie;
    ir = radices.lower_bound (r);
    while (ir != radices.upper_bound (r))
    {
        QTextStream (&sortie) << ir->second.doc () << "\\n";
        ++ir;
    }
    return sortie;
}

void TLexicum::lajoute (QString l, ListeAnalyses& la, AnalyseMorpho * am)
{
    la.insert (ListeAnalyses::value_type (l, am));
    // ajout éventuel de l'adverbe
    if (am->entree ()->getAdv ().count () > 0)
    {
        QString f = am->getQ ();
        QString dd;
        foreach (Tentree * e, am->entree ()->getAdv ())
        {
            if (e==NULL) continue;
            Traits * t;
            if (am->getTraits ()->contient (bitsComp))
            {
                t = bitsComp;
                dd = am->getD ();
            }
            else if (am->getTraits ()->contient (bitsSup))
            {
                t = bitsSup;
                dd = am->getD ();
                dd.chop (1); dd.append (0x0113);
            }
            else t = NULL;
            if (t!= NULL)
            {
                AnalyseMorpho * amadv = new AnalyseMorpho (f, t, e, dd);
                QString def = e->definition (lingua);
                la.insert (ListeAnalyses::value_type (def, amadv));
            }
        }
    }
}

/**
 * TLexicum::lanalyses
 * utilise une sortie sous
 * forme de liste d'analyses, dont tous
 * les items sont des instances de la
 * classe AnalyseMorpho.
 * TLexicum::lanalyses
 * supervise les recherches faites
 * par corAnalyses en une chaine d'appels successifs.
 * Il gère une éventuelle majuscule initiale
 * en tenant compte de l'option MajPert,
 * et appelle TLexicum::lanalysesE.
 * Qui cherche un possible enclitique,
 * et appelle TLexicum::lanalysesA qui fait
 * enfin l'assimilation des préfixes.
 * Chaque transformation est reproduite sur la forme avec quantités.
 * Les appels sucessifs permettent les combinaisons
 * de toutes les transformations :
 * par exemple, Adclamatioque
 */

ListeAnalyses TLexicum::lanalyses (QString forme, bool deb_phr)
{
    if ((forme.at (0) == 'V') && !(forme.contains ("v")))
        forme[0] = 'U';
    ListeAnalyses la = lanalysesE (forme); // passage direct à la détection d'enclitiques
    QString fq;
    // Majuscule initiale
    if (forme.at (0).isUpper () && (deb_phr || !MajPert))
    {
        ListeAnalyses adde = lanalysesE (forme.toLower ()); // Pour traiter les enclitiques et les assimilations
        for (std::multimap<QString, AnalyseMorpho*>::iterator it = adde.begin (); it != adde.end (); ++it)
        {
            it->second->setUpper ();
        }
        uniLanalyses (la, adde);
    }
    return la;
}

/* Traitement des suffixes */
ListeAnalyses TLexicum::lanalysesE (QString forme)
{
    ListeAnalyses la = lanalysesA (forme); // passage direct à la détection d'assimilations
    if (!forme.endsWith("e")) return la; // un raccourci.
    // Les enclitiques que, ne, ue ou ve se terminent tous par e !
    QString fsuff = "";
    QString suffixe = "";
    if (forme.endsWith ("ue"))
    {
        if (forme.endsWith ("que"))
        {
            fsuff = Ch::ini (forme, 3);
            suffixe = "qu\u0115";
        }
        else
        {
            fsuff = Ch::ini (forme, 2);
            suffixe = "v\u0115";
        }
    }
    else if (forme.endsWith ("ne"))
    {
        fsuff = Ch::ini (forme, 2);
        suffixe = "n\u0115";
    }
    else if (forme.endsWith ("ve"))
    {
        fsuff = Ch::ini (forme, 2);
        suffixe = "v\u0115";
    }
    if (!fsuff.isEmpty ())
    {
        ListeAnalyses adde = lanalysesA (fsuff); // Pour traiter les cas d'assimilations
        for (std::multimap<QString, AnalyseMorpho*>::iterator it = adde.begin (); it != adde.end (); ++it)
        {
            it->second->setSuff (suffixe);
        }
        uniLanalyses (la, adde);
    }
    return la;
}

/* Traitement des assimilations */
ListeAnalyses TLexicum::lanalysesA (QString forme)
{
    ListeAnalyses la = corAnalyses (forme); // passage direct au cœur du moteur d'analyses
    int nAss = 0;
    QString fpr = Ch::assimile (forme, nAss);
    if (fpr != forme)
    {
        ListeAnalyses adde = corAnalyses (fpr); // Attention ! pas de récursion ici, sinon boucle infinie.
        for (std::multimap<QString, AnalyseMorpho*>::iterator it = adde.begin (); it != adde.end (); ++it)
        {
            it->second->assimileq (nAss);
        }
        uniLanalyses (la, adde);
    }
    return la;
}

/**
 * cor (lat cor, cordis, angl core)
 * le cœur du moteur d'analyses
 *
 * TLexicum::corAnalyses cherche
 * la forme qui lui est donnée dans les listes
 * des formes canoniques, puis en la
 * décomposant en radical+désinence.
 * Il la cherche enfin dans la liste des irréguliers.
 * Il retourne une liste d'analyses
 * qui est gérée par le dispositif d'appels successifs
 * lanalyses - lanalysesE - lanalysesA
 */

ListeAnalyses TLexicum::corAnalyses (QString forme)
{
    int cnt = forme.toLower().count("v");
    // ramismes
    forme = Ch::jviu (forme);
    // variables
    ListeAnalyses la;
    lr liste_r;
    c2d::iterator id;
    c2r::iterator ir;
    c2i::iterator ii;
    lr::iterator ilr;
    Tentree * e;
    // recherche r+d inutile pour les pronoms et invariables
    // recherche de toutes les possibilités
    // radical.désinence initialisation des variables de la boucle
    QString r, d;
    int l = forme.length ();
    int i;
    // début de boucle
    if (l > 9) i = l-9;
    else i = 0;
    for (; i < l; ++i)
    {
        // calcul du radical et de la désinence
        r = forme.mid (0, i);
        d = forme.mid (i, l-i);
        // pour les radicaux nuls :
        if (r.isEmpty ())
            r = ".";
        // précaution : si la désinence n'existe pas, inutile de continuer.
        id = desinentiae.lower_bound (d);
        if (id != desinentiae.end () && QString::compare (id->second.graphie (), d, Qt::CaseInsensitive) != 0)
            continue;
        // recherche et mise en liste des radicaux
        liste_r.clear ();
        ir = radices.lower_bound (r);
        if (ir != radices.end () && QString::compare (ir->first, r, Qt::CaseSensitive) != 0)
            continue;
        while (ir != radices.upper_bound (r))
        {
            if ((!ir->second.en_i () || d.startsWith ("i")) && ((cnt==0)||(cnt == ir->second.grquant().toLower().count("v"))))
                liste_r.push_back (ir->second);
            ir++;
        }
        // recherche des désinences
        QString radq;
        while (id != desinentiae.upper_bound (d))
        {
            // boucle de recherche dans les radicaux
            ilr = liste_r.begin ();
            while (ilr != liste_r.end ())
            {
                // comparaison du modèle et du n° de radical
                if ((id->second.mdl ().contains (ilr->mdl ())) && (ilr->nr () == id->second.nr ()))
                {
                    e = ilr->entree ();
                    if (e != NULL)
                    {
                        if (!((e->getNombre () == 2) && (id->second.getTraits ()->numerus () == 1)))
                        {
                            radq = ilr->grquant ();
                            bool fauxReg = false;
                            foreach (Tirr * ir, e->irreguliers ())
                            {
                                fauxReg = fauxReg || (ir->exclusif () && ir->homomorphe (&id->second));
                            }
                            // éliminer les réguliers homomorphes d'un irrégulier exclusif
                            if (!fauxReg)
                            {
                                //en_i, ii = ī : allonger l'initiale de la désinence.
                                AnalyseMorpho * anMo = new AnalyseMorpho (radq, &id->second, e, "", ilr->en_i ());
#ifdef DEBOG
                                if (e->definition(lingua) == NULL)
                                    qDebug() << "pointeur null" << radq;
                                else
#endif
                                    lajoute (e->definition (lingua), la, anMo);
                            }
                        }
                    }
                }
                ++ilr;
            }
            ++id;
        }
    } // fin de boucle de recherce r.d

    ii = irregulares.lower_bound (forme);
    while (ii != irregulares.upper_bound (forme))
    {
        e = ii->second->entree ();
        if (e != NULL)
        {
            AnalyseMorpho * am = new AnalyseMorpho (ii->second->graphieq (), ii->second->getTraits (), e);
            lajoute (e->definition (lingua), la, am);
        }
        ++ii;
    }
    return la;
}

void TLexicum::uniLanalyses (ListeAnalyses& l1, ListeAnalyses& l2)
{
    // ajoute l2 à l1
    l1.insert (l2.begin (), l2.end ());
}

void TLexicum::deleteAnalyses (ListeAnalyses la)
{
    ListeAnalyses::iterator it;
    while (la.size () > 0)
    {
        it = la.begin ();
        la.erase (it);
    }
}

/**
 * met toutes les lettres en minuscules et laisse la
 * première tranquille.
 */
void minuscules (QString &f)
{
   bool prem = f[0].isUpper ();
   f = f.toLower();
   if (prem) f[0] = f[0].toUpper ();
}

QStringList TLexicum::lemmatiseM (QString f, bool mm, bool deb_phr)
{
    ListeAnalyses r = lanalyses (f, deb_phr);
    QStringList sortie;
    // les romains, en amont parce que les romains ne sont
    // pas dans le lexique.
	if (est_romain (f))
        sortie << QString ("%1 : %2").arg (f).arg (aRomano (f));
    // minuscules pour toutes les lettres sauf la première
    minuscules (f);
    // récup. des analyses
    if (r.empty () && sortie.isEmpty ())
    {
        return QStringList () << f;
    }

    // rassembler les clés
    QStringList cles;
    for (multimap<QString, AnalyseMorpho*>::iterator it = r.begin (); it != r.end (); ++it)
    {
       if (!cles.contains (it->first)) cles << it->first;
    }
    foreach (QString cle, cles)
    {
        QString lin = cle;
        if (mm)
        {
            pair<multimap<QString,AnalyseMorpho*>::iterator, multimap<QString,AnalyseMorpho*>::iterator> ret;
            ret = r.equal_range (cle);
            for (multimap<QString,AnalyseMorpho*>::iterator it=ret.first;it!=ret.second;++it)
                lin.append ( "<br/>\n&nbsp;&nbsp;" + it->second->humain ());
        }
        sortie << lin;
    }
    deleteAnalyses (r);
    return sortie;
}

QString sans_l (QString f)
{
    f = f.toLower ();
    f.replace (0x0101, 'a');
    f.replace (0x0103, 'a');
    f.replace (0x0113, 'e');
    f.replace (0x0115, 'e');
    f.replace (0x012b, 'i');
    f.replace (0x012d, 'i');
    f.replace (0x014d, 'o');
    f.replace (0x014f, 'o');
    f.replace (0x016b, 'u');
    f.replace (0x016d, 'u');
    return f;
}

bool sort_i (const QString &s1, const QString &s2)
{
    return sans_l (s1) < sans_l (s2);
}

QString TLexicum::lemmatiseTxt (QString txt, bool alpha, bool cumVocibus)
{
    // benchmark 1ère mesure
    // qint64 tDebut = QDateTime::currentDateTime ().toMSecsSinceEpoch ();
    bool deb_phr, deb_phr_suiv = true;
    QStringList formes = txt.split (Ch::reEspace, QString::SkipEmptyParts);
    QStringList lemmes;
    QStringList echecs;
    QStringList item;
    foreach (QString uox, formes)
    {
       deb_phr = deb_phr_suiv;
       if (uox.length () == 1 && ponctPhr.contains (uox))
       {
           deb_phr_suiv = true;
           continue;
       }
       else
       {
           deb_phr_suiv = ponctPhr.contains (uox.right (1));
           uox.remove (Ch::reNonAlphas);
       }
       if (uox.isEmpty () || Ch::estNombre (uox))
           continue;
       item = lemmatiseM (uox, false, deb_phr);
       if (item.at (0) == uox)
       {
           if (alpha) echecs << item;
           else lemmes << QString ("<span style=\"color:blue;\">%1</span>").arg (item.at (0));
       }
       else
       {
           if (cumVocibus)
               item[0].insert (0, "</div><strong><em>"+uox+"</em></strong>\n<div style=\"margin-left:20px\">");
           lemmes << item.join ("<br/>\n");
       }
    }
    if (alpha)
    {
        lemmes.removeDuplicates ();
        qSort (lemmes.begin (), lemmes.end (), sort_i);
    }
    if (cumVocibus)
    {
         lemmes[0].remove(0,6);
         lemmes << "</div>";
    }
    if (echecs.size () > 0)
    {
        lemmes << " " << "<strong>HAEC NON RECOGNITA:</strong>\n";
        echecs.removeDuplicates ();
        if (alpha)
            qSort (echecs.begin (), echecs.end (), sort_i);
        lemmes << echecs;
    }
    // benchmark 2ème mesure
    // qDebug () << QDateTime::currentDateTime ().toMSecsSinceEpoch () - tDebut;
    return lemmes.join ("<br/>\n");
}

QString TLexicum::par_position (QString f)
{
    bool maj = f.at (0).isUpper ();
    f = f.toLower ();
    foreach (Regle r, regles)
    {
        f.replace (r.first, r.second);
        f = f.trimmed ();
    }
    if (maj) f[0] = f[0].toUpper ();
    return f;
}

/**
 * Synthèse des formes adjectivales
 */
QStringList TLexicum::synthA (Tentree* e, int c, int n, int g, int d)
{
    // chercher dans les irréguliers
    QStringList retour;
    foreach (Tirr * ii, e->irreguliers ())
    if (        ii->getTraits ()->casus () == c
                && ii->getTraits ()->numerus () == n
                && ii->getTraits ()->genus () == g
                && ii->getTraits ()->gradus () == d)
        {
            retour << ii->graphieq ();
            if (ii->exclusif ())
                return retour;
        }
    c2d::iterator id;
    for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
        if ((id->second.mdl ().contains (modelesRad[e->modele()][id->second.nr ()-1]))
            && id->second.getTraits ()->casus () == c
            && id->second.getTraits ()->numerus () == n
            && id->second.getTraits ()->genus () == g
            && id->second.getTraits ()->gradus () == d)
        {
            foreach (QString r, e->radicalq (id->second.nr ()))
            if (Ch::consonnes.contains(id->second.grquant ()[0]))
                retour << Ch::allonge (r) + id->second.grquant ();
            else retour << r + id->second.grquant ();
        }
    return retour;
}

/**
 * Synthèse des formes nominales
 */
QStringList TLexicum::synthN (Tentree *e, int c, int n)
{
    // chercher dans les irréguliers
    QStringList retour;
    foreach (Tirr * ii, e->irreguliers ())
        if (ii->entree () == e
            && ii->getTraits ()->casus () == c
            && ii->getTraits ()->numerus () == n)
        {
            retour << ii->graphieq ();
            if (ii->exclusif ())
                return retour;
        }
    // puer
    if (e->modele () == 2 && c < 3 && n > 2)
    {
        retour << e->quantite ();
        return retour;
    }
    // désinences
    c2d::iterator id;
    for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
        if ((id->second.mdl ().contains (modelesRad[e->modele()][id->second.nr ()-1]))
            && id->second.getTraits ()->casus () == c
            && id->second.getTraits ()->numerus () == n)
        {
            foreach (QString r, e->radicalq (id->second.nr ()))
            {
                if (Ch::consonnes.contains(id->second.grquant ()[0]))
                    retour << Ch::allonge (r) + id->second.grquant ();
                else retour << r + id->second.grquant ();
            }
        }
    // neutres sans désinence
    if (retour.isEmpty ())
    {
        if (e->est_neutre () && n < 2 && c < 4)
            retour << e->quantite ();
        else if (n < 2 && (c < 3 && (e->modele () != 1)))
            retour << e->quantite ();
        else if (e->modele () == 1 && n < 2 && c == 1)
        {
            retour << e->quantite ();
        }
    }
    // vocatif type filī
    else if (e->modele () == 1 && c == 2 && n == 1 && e->canon ().endsWith ("ius"))
    {
        retour.clear ();
        retour << Ch::ini (e->radicalq (1)[0], 1) + "\u012b"; // "ī"
    }
    return retour;
}

/**
 * Synthèse des formes pronominales (uniquement irrégulières)
 */
QStringList TLexicum::synthP (Tentree* e, int c, int n, int g)
{
    // éliminer les sing. et pl. inexistants
    // chercher dans les irréguliers
    QStringList retour;
    if (((!e->aUnPluriel ()) && n==2) || ((!e->aUnSingulier ()) && n==1))
    {
        retour << "-";
        return retour;
    }
    foreach (Tirr * ii, e->irreguliers ())
    if (        ii->getTraits ()->casus () == c
                && ii->getTraits ()->numerus () == n
                && ii->getTraits ()->genus () == g)
        {
            retour << ii->graphieq ();
            if (ii->exclusif ())
                return retour;
        }
    c2d::iterator id;
    for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
        if ((id->second.mdl ().contains (modelesRad[e->modele()][id->second.nr ()-1]))
            && id->second.getTraits ()->casus () == c
            && id->second.getTraits ()->numerus () == n
            && id->second.getTraits ()->genus () == g)
        {
            foreach (QString r, e->radicalq (id->second.nr ()))
            if (Ch::consonnes.contains(id->second.grquant ()[0]))
                retour << Ch::allonge (r) + id->second.grquant ();
            else retour << r + id->second.grquant ();
        }
    return retour;
}

/**
 * Synthèse des formes verbales
 */
QStringList TLexicum::synthV (Tentree* e, int p, int n, int t, int m, int v)
{
    QStringList retours;
    // chercher dans les irréguliers
    bool exclusif = false;
    foreach (Tirr * ii, e->irreguliers ())
    {
        if (ii->getTraits ()->persona () == p
                && ii->getTraits ()->numerus () == n
                && ii->getTraits ()->tempus () == t
                && ii->getTraits ()->modus () == m
                && ii->getTraits ()->uox () == v)
        {
            exclusif = exclusif || ii->exclusif ();
            retours.append (ii->graphieq ());
        }
    }
    if (exclusif) return retours;
    // chercher dans les désinences
    c2d::iterator id;
    if ((v == 2 || e->est_deponent ()) && t > 3)
    {
        QStringList lpp;
        QStringList lsum;
        // si inf. passif, accusatif
        if (m == 4) lpp = synthPart (e, 3,1,1,4,2);
        // sinon, nominatif
        else lpp = synthPart (e, 1,1,n,4,2);
        if (lpp.isEmpty ()) return (QStringList () << "-");
        t = t - 3;
        for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
        {
            if (
                    id->second.mdl ().contains (22) // modèle de sum
                    && id->second.getTraits ()->persona () == p
                    && id->second.getTraits ()->numerus () == n
                    && id->second.getTraits ()->tempus () == t
                    && id->second.getTraits ()->modus () == m
               )
            {
                lsum << id->second.grquant ();
            }
        }
        foreach (QString pp, lpp)
        {
            foreach (QString sum, lsum)
            {
                 retours << pp + " " + sum;
            }
        }

    }
    else // actif
    {
        QList<Tdes> des;
        for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
            if (
                id->second.mdl ().contains (modelesRad[e->modele()][id->second.nr()-1])
                && id->second.getTraits ()->persona () == p
                && id->second.getTraits ()->numerus () == n
                && id->second.getTraits ()->tempus () == t
                && id->second.getTraits ()->modus () == m
                && id->second.getTraits ()->uox () == v
               )
            {
                des.append (id->second);
            }
        for (int j=0;j<des.length();++j)
        {
            QStringList lr = e->radicalq (des[j].nr());
            if (lr.empty () || lr[0].isEmpty ())
                retours.append("-");
            else for (int i=0;i<lr.length();++i) // radicaux multiples séparés par ','
            {
                if (lr[i] == ".") lr[i] = "";
                if (Ch::consonnes.contains(des[j].grquant ()[0]))
                    retours << Ch::allonge (lr[i]) + des[j].grquant ();
                else retours << lr[i] + des[j].grquant ();
            }
        }

    }
    return retours;
}

/**
 * Synthèse des formes participiales
 */
QStringList TLexicum::synthPart (Tentree* e, int c, int g, int n, int t, int v)
{
    QStringList retour;
    c2d::iterator id;
    for (id = desinentiae.begin (); id != desinentiae.end (); ++id)
        if (id->second.mdl ().contains (modelesRad[e->modele()][id->second.nr ()-1])
            && id->second.getTraits ()->casus () == c
            && id->second.getTraits ()->numerus () == n
            && id->second.getTraits ()->genus () == g
            && id->second.getTraits ()->tempus () == t
            && id->second.getTraits ()->modus () == 5
            && (id->second.getTraits ()->uox () == v)
            )
        {
            foreach (QString r, e->radicalq (id->second.nr ()))
            {
                if (r.isEmpty ())
                    continue;
                if (r == ".")
                    retour << id->second.grquant ();
                else retour << r + id->second.grquant ();
            }
        }
    return retour;
}

QString TLexicum::flechis (Tentree* e)
{
    QString sortie;
    QTextStream ss (&sortie);
    QString lvide = "<tr><td>&nbsp;</td></tr>";
    int ic; // variable pour les boucles de cas
    ss << "<table border=1 cellspacing=\"0\" cellpadding=\"5\">";
    ss << "<caption>" << e->quantite () << ", " << e->doc ();
    ss << " &nbsp;---&gt;&nbsp; " << e->grmodele () << "</caption>\n";
    switch (e->pars ())
    {
        case 'n':
        {
            ss << "<tr><td><strong>sing.</strong></td></tr>\n";
            if (e->aUnSingulier ())
                for (ic=1;ic<7;++ic)
                {
                    ss << "<tr><td>" << cass[ic] << "</td><td>" << synthN (e, ic, 1).join (",") << "</td></tr>\n";
                }
            ss << "<tr><td><strong>plur.</strong></td></tr>\n";
            if (e->aUnPluriel ())
                for (ic=1;ic < 7;++ic)
                {
                    ss << "<tr><td>" << cass[ic] << "</td><td>" << synthN (e, ic, 2).join (",") << "</td></tr>\n";
                }
            ss << "</table>";
            return sortie;
        }
    case 'a':
        {
            QString chM, chF, chN;
            int iid;
            for (int id=1; id<4; ++id)
            {
                if (id==1) iid=0;
                else
                {
                    iid=id;
                    if (e->radicalq(id).empty()) continue; // S'il n'y a pas de comp. ou de sup. on saute.
                }
                ss << "<tr><td><strong>" << degres[id].toUpper() << "</strong></td></ŧr>" << endl;
                int in = 1;
                if (e->getNombre () == 2) in=2; // pour tenir compte de "pl."
                for (; in<3; ++in)
                {
                    ss << "<tr><td><strong>" << nombres[in] << "</strong></td><td>M.</td><td>F.</td><td>N.</td></ŧr>" << endl;
                    for (int ic = 1;ic < 7;++ic)
                    {
                        // synthA (cas, nombre, genre, degré)
                        chM = synthA (e, ic,in,1,iid).join (",");
                        chF = synthA (e, ic,in,2,iid).join (",");
                        chN = synthA (e, ic,in,3,iid).join (",");
                        ss << "<tr><td>" << cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
                    }
                    if (e->getNombre () == 1) break;
                }
                ss << lvide;
            }
        ss << "</table>";
        return sortie;
    }
        case 'p':
        {
            QString chM, chF, chN;
            ss << "<tr><td><strong>sing.</strong></td><td>M.</td><td>F.</td><td>N.</td></ŧr>" << endl;
            int ic = 1;
            if (e->aUnSingulier ())
                for (;ic<7;++ic)
                {
                    chM = synthP (e, ic, 1, 1).join (",");
                    chF = synthP (e, ic, 1, 2).join (",");
                    chN = synthP (e, ic, 1, 3).join (",");
                    ss << "<tr><td>" << cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
                }
            ss << "<tr><td><strong>plur.</strong></td></td><td>M.</td><td>F.</td><td>N.</td></tr>";
            if (e->aUnPluriel ())
                for (ic=1;ic<7;++ic)
                {
                    chM = synthP (e, ic, 2, 1).join (",");
                    chF = synthP (e, ic, 2, 2).join (",");
                    chN = synthP (e, ic, 2, 3).join (",");
                    ss << "<tr><td>" << cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
                }
            ss << "</table>";
            return sortie;
        }
        case 'v':
        {
            int ip, in;
            QString col1, col2, col3;
            ss << "<tr><td></td><td><strong>ACTIVA VOX</strong></td></tr>"<< endl;
            ss << "<tr><td></td><td><strong>indicatiuus</strong></td>" << endl;
            ss << "<tr><td></td><td> <strong>praes.</strong></td><td><strong>fut.</strong></td><td><strong>impf.</strong></td></tr>";
            for (in=1;in<3;in++)
                  for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << personnes[ip] << "</td><td>" << synthV (e, ip,in,1,1,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,2,1,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,3,1,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong> pf.</strong></td><td><strong>fut. ant.</strong></td><td><strong>pqp.</strong></td></tr>";
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << personnes[ip] << "</td><td>" <<  synthV (e, ip,in,4,1,1).join (",") << "</td>";
                    ss << "<td>" <<  synthV (e, ip,in,5,1,1).join (",") << "</td>";
                    ss << "<td>" <<  synthV (e, ip,in,6,1,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong>coniunctiuus</strong></td></tr>" << endl;
            ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>impf.</strong></td></tr>" << endl;
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << personnes[ip] << "</td><td>" << synthV (e, ip,in,1,2,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,3,2,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>pqp.</strong></td></tr>" << endl;
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << personnes[ip] << "</td><td>" << synthV (e, ip,in,4,2,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,6,2,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong>imperatiuus</strong></td></tr>" << endl;
            ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>fut.</strong></td></tr>";
            ss << "<tr></td><td><td>" << synthV (e,2,1,1,3,1).join (",") << "</td>";
            ss << "<td>" << synthV (e,2,1,2,3,1).join (",") << "</td></tr>";
            ss << "<tr></td><td><td>" <<  synthV (e,2,2,1,3,1).join (",") << "</td>";
            ss << "<td>" <<  synthV (e,2,2,2,3,1).join (",") << "</td></tr>";
            ss << "<tr></td><td><td><strong>infinitiuus</strong></td></tr>" << endl;
            ss << "<tr></td><td><td><strong>praes.</strong></td><td><strong>pf.</strong></td></tr>";
            ss << "<tr><td></td><td>" <<  synthV (e,0,0,1,4,1).join (",") << "</td><td>" << synthV (e,0,0,4,4,1).join (",") << "</td></tr>";
            ss << "<tr><td></td><td><strong>part. pr.</strong></td></tr>" << endl;
            ss << "<tr><td><strong>sing.</strong></td></tr>";
                // ans|1|3|1|0|0|1|5|1|17|1
                int ic;
                for (in=1;in<3;in++)
                {
                    for (ic=1;ic<7;++ic)
                    {
                        col1 = synthPart (e,ic,1,in,1,1).join (",");
                        col2 = synthPart (e,ic,2,in,1,1).join (",");
                        col3 = synthPart (e,ic,3,in,1,1).join (",");
                        ss << "<tr><td>" << cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                    if (in == 1) ss << "<tr><td><strong>plur.</strong></td></tr>" << endl;
                }
            ss << "<tr><td></td><td><strong>part. fut.</strong></td></tr>" << endl;
            ss << "<tr><td><strong>sing.</strong></td></tr>";
                // ans|1|3|1|0|0|1|5|1|17|1
            for (in=1;in<3;in++)
            {
                for (ic=1;ic<7;++ic)
                {
                    col1 = synthPart (e,ic,1,in,2,1).join (",");
                    col2 = synthPart (e,ic,2,in,2,1).join (",");
                    col3 = synthPart (e,ic,3,in,2,1).join (",");
                    ss << "<tr><td>" << cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                }
                if (in == 1) ss << "<tr><td><strong>plur.</strong></td></tr>" << endl;
            }
            if (e->est_deponent () || e->modele () == 22 || e->est_intransitif ())
                ss << "<tr><td colspan=\"3\">deponentibus non passiua uox, neque uerbo sum, neque intransitiuibus.</td></tr>" << endl;
            else
            {
                ss << lvide;
                ss << "<tr><td></td><td><strong>PASSIVA VOX</strong></td></tr>" << endl;
                ss << "<tr><td></td><td><strong>indicatiuus</strong></td></tr>" << endl;
                ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>fut.</strong></td><td><strong>impf.</strong></td></tr>";
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,1,1,2).join (",");
                        col2 = synthV (e, ip,in,2,1,2).join (",");
                        col3 = synthV (e, ip,in,3,1,2).join (",");
                        ss << "<tr><td>" << personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>fut. ant.</strong></td><td><strong>pqp</strong></td></tr>";
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,4,1,2).join (",");
                        col2 = synthV (e, ip,in,5,1,2).join (",");
                        col3 = synthV (e, ip,in,6,1,2).join (",");
                        ss << "<tr><td>" << personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>coniunctiuus</strong></td></tr>" << endl;
                ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>impf.</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,1,2,2).join (",");
                        col2 = synthV (e, ip,in,3,2,2).join (",");
                        ss << "<tr><td>" << personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>pqp</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,4,2,2).join (",");
                        col2 = synthV (e, ip,in,6,2,2).join (",");
                        ss << "<tr><td>" << personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>imperatiuus praes.</strong></td></tr>" << endl;
                ss << "<tr><td></td><td>" << synthV (e,2,1,1,3,2).join (",") << "</td></tr>";
                ss << "<tr><td></td><td>" << synthV (e,2,2,1,3,2).join (",") << "</td></tr>";
                ss << "<tr><td></td><td><strong>infinitiuus</strong></td></tr>" << endl;
                ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>pf.</strong></td></tr>";
                col1 = synthV (e,0,0,1,4,2).join (",");
                col2 = synthV (e,0,0,4,4,2).join (",");
                ss << "<tr><td></td><td>" << col1 << "</td><td>" << col2 << "</td></tr>";
                ss << "<tr><td></td><td><strong>part. pf.</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                {
                    for (ic=1;ic<7;++ic)
                    {
                        col1 = synthPart (e,ic,1,in,4,2).join (",");
                        col2 = synthPart (e,ic,2,in,4,2).join (",");
                        col3 = synthPart (e,ic,3,in,4,2).join (",");
                        ss << "<tr><td>" << cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                    if (in == 1) ss << "<tr><td><strong>plur.</strong></td></tr>" << endl;
                }
            }
            ss << "</table>";
            return sortie;
        }
        case 'i':
        {
            return sortie;
        }
        default:
            return "nequeo";
    }
}

// scansion

QStringList TLexicum::formeq (QString forme, bool *nonTrouve, bool deb_phr)
{
    QStringList lforme;
    *nonTrouve = true;
    if (forme.isEmpty ())
    {
        return lforme;
    }
    ListeAnalyses la = lanalyses (forme, deb_phr);
    if (la.empty () && lforme.empty ())
    {
        forme = par_position (forme);
        lforme.append (forme);
        return lforme;
    }
    *nonTrouve = false;
    for (std::multimap<QString, AnalyseMorpho*>::iterator it = la.begin (); it != la.end (); ++it)
    {
        QString fq = it->second->getForm ();
        lforme.append (fq);
    }
    lforme.removeDuplicates ();
    deleteAnalyses (la);
    return lforme;
}


// écrit le texte scandé.
QString TLexicum::scandeTxt (QString texte, bool stats)
{
    QString schemaMetric;
    QMap <QString, int> freqMetric;
    bool deb_phr;
    QStringList formes;
    QStringList separ;
    // position
    QStringList aff;
    QStringList lignes = texte.split ("\n");
    int io;
    foreach (QString ligne, lignes)
    {
        io = ligne.indexOf (Ch::reLettres);
        if (ligne.isEmpty() || (io < 0))
        {
            aff.append (ligne + "<br />\n");
            // C'est une ligne vide ou ne contenant pas de lettre :
            // je la laisse comme elle est !
            continue;
        }
        formes.clear ();
        separ.clear();
        if (io > 0)
        {
            separ << ligne.left (io);
            ligne.remove (0,io);
        }
        else separ << "";
        // Maintenant je suis sûr que ligne commence par un mot
        while (ligne.length () > 0)
        {
            io = ligne.indexOf (Ch::reNonAlphas);
            // io repère l'indice du premier caractère qui n'est pas une lettre
            if (io < 0)
            {
                // Je suis arrivé à la fin de la ligne
                formes << ligne;
                ligne = "";
                continue;
            }
            else
            {
                formes << ligne.left (io);
                // c'est le mot
                ligne.remove (0,io);
                // ligne commence maintenant par un caractère qui n'est pas une lettre
            }
            io = ligne.indexOf (Ch::reLettres);
            // io repère l'indice de la prochaine lettre
            if (io < 0)
            {
                separ << ligne;
                ligne = "";
            }
            else
            {
                separ << ligne.left (io);
                // C'est le séparateur
                ligne.remove (0,io);
                // A nouveau, ligne commence par une lettre
            }
        }
        if (separ.size () == formes.size ()) separ << "";
        // J'ai maintenant une liste de formes et une liste de séparateurs
        // la ligne d'origine est la concaténation de separ[i]+formes[i]
        // J'ai toujours un dernier séparateur, éventuellement vide.
        // La scansion peut commencer !
        bool nonTr, nonTrSuiv;
        QStringList lforme;
        QStringList lfs = formeq (formes[0], &nonTrSuiv, true);
        schemaMetric = "";
        for (int i=0;i<formes.length();++i)
        {
            aff.append (separ[i]);
            lforme = lfs;
            nonTr = nonTrSuiv;
            if (i < formes.length ()-1)
            {
                deb_phr = separ[i+1].contains (Ch::rePonct);

                lfs = formeq (formes[i+1], &nonTrSuiv, deb_phr);
                if (Ch::consonnes.contains(lfs[0].at(0).toLower()))
                    for (int j=0;j<lforme.length();++j)
                        Ch::allonge (&lforme[j]);
                else for (int j=0;j<lforme.length();++j)
                    Ch::elide (&lforme[j]);
            }
            lforme.removeDuplicates ();
            // C'est le bon moment pour extraire le schéma métrique
            if (stats)
            {
                if (nonTr) schemaMetric.append(lforme[0]+" ");
            else
            {
                QString schMet = Ch::versPedeCerto(lforme[0]);
                if (lforme.length () > 1) for(int ii=1;ii<lforme.length(); ii++)
                {
                    QString schMet2 = Ch::versPedeCerto(lforme[ii]);
                    if (schMet.size()!=schMet2.size())
                    {
                        schMet = "@" + lforme[0];
                        continue;
                    }
                    else for (int j=0; j<schMet.size(); j++)
                        if (schMet[j] != schMet2[j]) schMet[j]='*';
                    // En cas de réponse multiple, je marque comme communes les voyelles qui diffèrent
                }
                schemaMetric.append(schMet+" ");
            }
            }
            // ajouter des parenthèses pour les analyses multiples
            if (lforme.length () > 1)
            {
                lforme[1].prepend ('(');
                lforme[lforme.length () - 1].append (')');
            }
            if (nonTr) aff.append ("<em>"+lforme.join (" ")+"</em>");
            else aff.append (lforme.join (" "));
            // pour les analyses multiples, je dois insérer des espaces.
        }
        aff.append (separ[formes.length ()] + "<br />\n");
        // Je termine la ligne par le dernier séparateur et un saut de ligne.
        if (stats)
        {
            schemaMetric.replace('-', "u");
            schemaMetric.replace('+', "-");
            schemaMetric.replace('*', "-\u0306");
            aff.append (schemaMetric + "<br>\n");
            schemaMetric.remove(" ");
            schemaMetric.remove("`");
            /* Pour ignorer la longueur de la dernière voyelle
               if (!schemaMetric.endsWith("\u0306"))
               {
               schemaMetric[schemaMetric.length()-1]='-';
               schemaMetric.append("\u0306");
               }
               */
            freqMetric[schemaMetric] +=1;
        }
    }
    if (stats)
    {
        // Il me reste à trier les freqMetric
        formes.clear();
        foreach (QString schM, freqMetric.keys())
            if (freqMetric[schM] > 1) // Je ne garde que les schéma qui apparaissent plus d'une fois.
            {
                int n = freqMetric[schM] + 10000;
                QString numero;
                numero.setNum (n);
                numero = numero.mid (1);
                formes << numero + " : " + schM ;
            }
        formes.sort();
        aff.prepend("------------<br/>\n"); // Pour séparer la liste du texte.
        for (int i=0; i<formes.size(); i++)
        {
            QString lg = formes[i];
            while (lg[0]=='0') lg=lg.mid(1);
            aff.prepend (lg + "<br/>\n");
            // En faisant un prepend, j'inverse l'ordre : 
            // le plus fréquent finira premier
        }
    }
    return aff.join ("");
}

/******************
 *    FRÉQUENCES
 ******************/

bool tri (const QString &s1, const QString &s2)
{
    return s1.toLower() > s2.toLower();
}

QStringList TLexicum::frequences (QString txt)
{
    // J'essaie d'=échanger comptage et lemmatisation Ph.
    QStringList formes = txt.split (Ch::reEspace);
    QHash <QString, int> freq; // Occurrences par formes
    QStringList res;
    QString forme;
    int c = formes.count ();
    for (int i= 0;i<c;++i)
    {
        forme = formes.at (i);
        if (forme.isEmpty () || forme.toInt ()) continue;
        // supprimer les ponctuations
        int pos = Ch::reAlphas.indexIn (forme);
        if (pos < 0) continue;
        forme = Ch::reAlphas.cap (1);
        if ((i == 0) || formes[i-1].contains (Ch::rePonct)) forme = "*" + forme;
        freq[forme]++; // je suppose que le créateur d'entiers l'initialise à 0
        /* sinon prendre des précautons :
           if (freq.keys ().contains (r))
           freq[r] = freq[r]+1;
           else freq[r] = 1;
        */
    }
    QHash<QString,int> lemOcc; // Nombre d'occurrences par lemme
    // REMARQUE TODO : QList::swap pourrait servir pour le tri.
    QHash<QString,QStringList> lemFormUnic; // liste de formes uniques = par lemme
    QHash<QString,QStringList> lemFormAmb; // Liste de formes ambiguës par lemme
    QHash<QString,QStringList> formLemAmb; // Liste de lemmes par forme ambiguë
    foreach (forme, freq.keys())
    {
        if (forme.startsWith("*"))
        {
            QString forme2 = forme.mid(1);
            res = lemmatiseM (forme2, false, true);
        }
        else res = lemmatiseM (forme, false, false);
        int occ = freq[forme];
        if (res.count()== 1)
        {
            lemFormUnic[res.at(0)].append(forme);
            lemOcc[res.at(0)] += occ;
        }
        else
        {
            for (int i= 0;i<res.count();i++)
            {
                lemFormAmb[res.at(i)].append(forme);
                lemOcc[res.at(i)] += occ;
            }
            formLemAmb[forme] = res;
        }
    }
    formes.clear ();
    formes << lemFormUnic.keys();
    formes << lemFormAmb.keys();
    formes.removeDuplicates();
    QStringList sortie;
    // formater les nombres pour tri inverse
    int nUnic;
    float nTotLem;
    int nAmb;
    float xAmb;
    foreach (QString lemme, formes)
    {
        nUnic = 0;
        foreach (forme, lemFormUnic[lemme])
        {
            nUnic += freq[forme];
        }
        nAmb = 0;
        xAmb = 0;
        foreach (forme, lemFormAmb[lemme])
        { // Le lemme considéré a des formes qu'il partage avec d'autres lemmes
            nTotLem = 0;
            foreach (QString lem, formLemAmb[forme])
                nTotLem += lemOcc[lem];
            // Je somme les occurrences de tous les lemmes qui se rattachent à ma forme
            nAmb += freq[forme];
            xAmb += freq[forme] * lemOcc[lemme] / nTotLem;
            // J'attribue une contribution de ma forme au lemme au prorata des occ des lemmes
        }
        int n = xAmb + nUnic + 10000.5;
        QString numero;
        numero.setNum (n);
        numero = numero.mid (1);
        n = xAmb+0.5; // pour faire un arrondi et pas une troncature
        sortie << QString ("%1 (%2, %3, %5)\t%4<br/>\n").arg (numero).arg(nUnic).arg(nAmb).arg (lemme).arg(n);
    }
    qSort(sortie.begin(), sortie.end(), tri);
    // déformatage des nombres
    int cs = sortie.count ();
    for (int i= 0;i<cs;++i)
    {
        QString ls = sortie.at (i);
        int z = 0;
        while (ls.at (z) == '0') ++z;
        ls = ls.mid(z);
        if (ls.at(0) == ' ') ls.prepend("&lt;1");
        sortie[i] = ls;
    }
    sortie.insert (0, "légende : n (a, b, c)<br/>\n");
    sortie.insert (1, "n = a+c<br/>\n");
    sortie.insert (2, "a = nombre de formes rattachées seulement à ce lemme<br/>\n");
    sortie.insert (3, "b = nombre de formes ambigu\u00ebs (partagées par plusieurs lemmes)<br/>\n");
    sortie.insert (4, "c = nombre probable de formes ambigu\u00ebs rattachées à ce lemme<br/>\n");
    sortie.insert (5, "------------<br/>\n");
    return sortie;
}
