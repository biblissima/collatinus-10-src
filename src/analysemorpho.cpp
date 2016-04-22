/*
 *                          analysemorpho.cpp
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


#include "analysemorpho.h"
#include <QDebug>

/*
AnalyseMorpho::AnalyseMorpho(QObject *parent) :
    QObject(parent)
{
}
*/

/********************/
/*   ANALYSEMORPHO  */
/********************/

AnalyseMorpho::AnalyseMorpho (QString rq, Desinence * d, Entree * en,
                              QString suff, bool ii, QObject *parent) :
    QObject(parent)
{
    des = d;
    e = en;
    q = rq;
    dq = des->grquant ();
    suffixe = suff;
/*    if (ii)
    {
        q.chop(1);
        dq.remove(0,1);
        dq = "ī"+dq;
    }*/
    db_i = ii;
    traits = d->getTraits ();
}

AnalyseMorpho::AnalyseMorpho (QString rq, Traits * t, Entree * en, QString ddq, QObject *parent) :
    QObject(parent)
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
//    delete traits;
//    delete des;
}

const QString AnalyseMorpho::consonnes = "bcdfgjklmnpqrstvxz";
const QString AnalyseMorpho::voyelles  = QString::fromUtf8 ("a\u0101\u0103e\u0113\u0115i\u012b\u012do\u014d\u014fu\u016b\u016dy\u0233\u045e");
const QString AnalyseMorpho::longues  = QString::fromUtf8 ("\\x0101\\x0113\\x012b\\x014d\\x016b\u045e");
const QChar AnalyseMorpho::separSyll = 0x02CC;

const QStringList AnalyseMorpho::assimAnteq = QStringList ()
    << "\u0101dc"<<"\u0101df"<<"\u0101dg"<<"\u0101dl"<<"\u0101dp"<<"\u0101dq"<<"\u0101dr"
    <<"\u0101dst"<<"\u0101dsc"<<"\u0101dsp"<<"\u0101ds"<<"\u0101dt"<<"\u0101ps"<<"c\u014dnl"
    <<"c\u014dnm"<<"c\u014dnp"<<"c\u014dnr"<<"\u0113xs"<<"\u012bnb"<<"\u012bnl"<<"\u012bnm"
    <<"\u012bnp"<<"\u012bnr"<<"\u014dbc"<<"\u014dbf"<<"\u014dbp"<<"\u014dps"<<"s\u016bbc"
    <<"s\u016bbf"<<"s\u016bbg"<<"s\u016bbm"<<"s\u016bbp"<<"s\u016bpt";
const QStringList AnalyseMorpho::assimPostq = QStringList ()
    << "\u0101cc"<<"\u0101ff"<<"\u0101gg"<<"\u0101ll"<<"\u0101pp"<<"\u0101cq"<<"\u0101rr"
    <<"\u0101st"<<"\u0101sc"<<"\u0101sp"<<"\u0101ss"<<"\u0101tt"<<"\u0101bs"<<"c\u014dll"
    <<"c\u014dmm"<<"c\u014dmp"<<"c\u014drr"<<"\u0113x"<<"\u012bmb"<<"\u012bll"<<"\u012bmm"
    <<"\u012bmp"<<"\u012brr"<<"\u014dcc"<<"\u014dff"<<"\u014dpp"<<"\u014dbs"<<"s\u016bcc"
    <<"s\u016bff"<<"s\u016bgg"<<"s\u016bmm"<<"s\u016bpp"<<"s\u016bbt";
const QStringList AnalyseMorpho::assimAnte = QStringList ()
    << "adc"<<"adf"<<"adg"<<"adl"<<"adp"<<"adq"<<"adr"<<"adst"<<"adsc"<<"adsp"<<"ads"
    <<"adt"<<"aps"<<"conl"<<"conm"<<"conp"<<"conr"<<"exs"<<"inb"<<"inl"<<"inm"<<"inp"
    <<"inr"<<"obc"<<"obf"<<"obp"<<"ops"<<"subc"<<"subf"<<"subg"<<"subm"<<"subp"<<"supt";
const QStringList AnalyseMorpho::assimPost = QStringList ()
    <<"acc"<<"aff"<<"agg"<<"all"<<"app"<<"acq"<<"arr"<<"ast"<<"asc"<<"asp"<<"ass"
    <<"att"<<"abs"<<"coll"<<"comm"<<"comp"<<"corr"<<"ex"<<"imb"<<"ill"<<"imm"<<"imp"
    <<"irr"<<"occ"<<"off"<<"opp"<<"obs"<<"succ"<<"suff"<<"sugg"<<"summ"<<"supp"<<"subt";

QString AnalyseMorpho::assimile (QString f, int &i)
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

void AnalyseMorpho::assimileq (int i)
{
    if (q.startsWith (assimAnteq[i]))
        q = q.replace (QRegExp ("^"+assimAnteq[i]), assimPostq[i]);
    else
        if (q.startsWith (assimPostq[i]))
        q = q.replace (QRegExp ("^"+assimPostq[i]), assimAnteq[i]);
}

QString AnalyseMorpho::allonge (QString f)
{
    int taille = f.size();
    // Je sais que le morceau à attacher commence par une consonne.
    if (consonnes.contains (f.at (taille-1))
        && !QString ("\u0101e \u0101u \u0113u \u014de").contains (f.mid (taille-3, 2).toLower()))
    {
        f.replace (QRegExp ("[a\u0103](["+consonnes+"])$"), "\u0101\\1");
        f.replace (QRegExp ("[e\u0115](["+consonnes+"])$"), "\u0113\\1");
        f.replace (QRegExp ("[i\u012d](["+consonnes+"])$"), "\u012b\\1");
        f.replace (QRegExp ("[o\u014F](["+consonnes+"])$"), "\u014d\\1");
        f.replace (QRegExp ("[u\u016d](["+consonnes+"])$"), "\u016b\\1");
        f.replace (QRegExp ("[y\u0233](["+consonnes+"])$"), "\u045e\\1");
        f.replace (QRegExp ("[A\u0102](["+consonnes+"])$"), "\u0100\\1");
        f.replace (QRegExp ("[E\u0114](["+consonnes+"])$"), "\u0112\\1");
        f.replace (QRegExp ("[I\u012c](["+consonnes+"])$"), "\u012a\\1");
        f.replace (QRegExp ("[O\u014e](["+consonnes+"])$"), "\u014c\\1");
        f.replace (QRegExp ("[U\u016c](["+consonnes+"])$"), "\u016a\\1");
        f.replace (QRegExp ("[Y\u0232](["+consonnes+"])$"), "\u040e\\1");
    }
    return f;
}

QString AnalyseMorpho::transforme (QString k)
{
    k.replace("āe","æ+"); k.replace("ōe","œ+");
    k.replace("ăe","æ-");
    k.replace("Āe","Æ+"); k.replace("Ōe","Œ+");
    // Je remplace les longues par +, les brèves par - et les communes par *
    // minuscules
    k.replace (0x0101, "a+"); k.replace (0x0103, "a-");  // ā ă
    k.replace (0x0113, "e+"); k.replace (0x0115, "e-");  // ē ĕ
    k.replace (0x012b, "i+"); k.replace (0x012d, "i-");  // ī ĭ
    k.replace (0x014d, "o+"); k.replace (0x014f, "o-");  // ō ŏ
    k.replace (0x016b, "u+"); k.replace (0x016d, "u-");  // ū ŭ
    k.replace (0x0233, "y+"); k.replace (0x045e, "y-");  // ȳ ў
    // majuscule
    k.replace (0x0100, "A+"); k.replace (0x0102, "A-");  // Ā Ă
    k.replace (0x0112, "E+"); k.replace (0x0114, "E-");  // Ē Ĕ
    k.replace (0x012a, "I+"); k.replace (0x012c, "I-");  // Ī Ĭ
    k.replace (0x014c, "O+"); k.replace (0x014e, "O-");  // Ō Ŏ
    k.replace (0x016a, "U+"); k.replace (0x016c, "U-");  // Ū Ŭ
    k.replace (0x0232, "Y+"); k.replace (0x040e, "Y-");  // Ȳ Ў
    // "+" + combining breve = voyelle commune
    k.replace ("+\u0306", "*");
    k.replace(0x1ee5, "u"); // suppression du u-exponctué.
    return k;
}

QString AnalyseMorpho::accentue (QString l)
{
    if ((l == "œ") ||(l == "Œ")) return l+"\u0301";
    if (l == "æ") return "ǽ";
    if (l == "Æ") return "Ǽ";
    int a = l[0].unicode();
    switch (a) {
    case 97 : return "á";
    case 101 : return "é";
    case 105 : return "í";
    case 111 : return "ó";
    case 117 : return "ú";
    case 121 : return "ý";
    case 65 : return "Á";
    case 69 : return "É";
    case 73 : return "Í";
    case 79 : return "Ó";
    case 85 : return "Ú";
    case 89 : return "Ý";
        break;
    default:
        return l;
        break;
    }
}

QString AnalyseMorpho::ajoutSuff(QString fq, int accent)
{
    bool cesure = false;
    bool sansAccent = false;
    if (accent > 3)
    {
        cesure = true;
        accent -= 4;
    }
    if (accent>0)
    {
        QString signes = "+-*";
        fq = transforme(fq);
        int l = fq.count('+') + fq.count('-') + fq.count('*'); // nombre de syllabes.
        int i = fq.size() - 1;
        if (suffixe.isEmpty ())
        {
        // Sans suffixe, l'accent est sur l'avant-dernière voyelle si elle n'est pas brève
            if (l>2)
            {
                while (!signes.contains(fq[i])) i-=1;
                i-=1;
                while (!signes.contains(fq[i])) i-=1;
                sansAccent = (fq[i] == '*') && (accent == 3);
                // La pénultième est commune et je la considère comme ambiguë = pas d'accent.
                if ((fq[i] == '-') || ((fq[i] == '*') && (accent == 2)))
                {
                    // Remonter à l'antépénultième
                    i-=1;
                    while (!signes.contains(fq[i])) i-=1;
                }
                if (!sansAccent)
                {
                    if (i>1)
                        fq = fq.mid(0, i-1) + accentue(fq.mid(i-1,1)) + fq.mid(i);
                    else fq = accentue(fq.mid(i-1,1)) + fq.mid(i);
                }
            }
        }
        else
        {
            // Avec suffixe, l'accent est sur la dernière (avant collage)
            if (l>1)
            {
                while (!signes.contains(fq[i])) i-=1;
                if (i>1)
                    fq = fq.mid(0, i-1) + accentue(fq.mid(i-1,1)) + fq.mid(i);
                else fq = accentue(fq.mid(i-1,1)) + fq.mid(i);
            }
            fq = fq + suffixe + "-";
            fq[fq.size()-2] = 'e'; // ôte le e-bref.
            l += 1;
        }
        // L'entier i pointe sur la longueur de la voyelle accentuée, sauf si j'ai ajouté un \u0301.
        // Si je veux marquer la syllabe accentuée en gras, c'est ici !
        if ((l>1) && cesure)
        {
            // Il y a au moins deux syllabes que je veux séparer
            int j = fq.size() -1;
            while (!signes.contains((fq[j])) && (j>0)) j-=1;
            int k = j;
            j -= 2;
            while (!signes.contains((fq[j])) && (j>0)) j-=1;
            while (j>0)
            {
                // la césure doit tomber quelque part entre j et k
                if (k == j+2) fq.insert(j,separSyll); // Il n'y a que la voyelle (et une quantité)
                else
                {
                    int nbCons = 0;
                    for (int n = j+1; n < k-1; n++)
                        if (consonnes.contains(fq[n]) || (fq[n]=='h')) nbCons +=1;
                    // J'ai le nombre de consonnes
                    if (nbCons == 0) fq.insert(k-1,separSyll);
                    else
                    {
                        while (!consonnes.contains(fq[k]) && (fq[k]!='h')) k-=1;
                        if (nbCons == 1) fq.insert(k,separSyll);
                        else
                        {
                            // c'est plus compliqué car j'ai au moins deux consonnes...
                            bool remonte = ((fq[k]=='l') && (fq[k-1]!='l'));
                            remonte = remonte || ((fq[k]=='r') && (fq[k-1]!='r'));
                            remonte = remonte || (fq[k]=='h');
                            if (remonte) k-=1;
                            remonte = ((fq[k]=='c') && (fq[k-1]=='s') &&
                                    (fq[k+1]!='a') && (fq[k+1]!='o') && (fq[k+1]!='u') && (fq[k+1]!='h'));
                            remonte = remonte || ((fq[k]=='p') && (fq[k-1]=='s'));
                            // remonte = remonte || ((fq[k]=='t') && (fq[k-1]=='s'));
                            remonte = remonte || ((fq[k]=='n') && (fq[k-1]=='g'));
                            if (remonte) k-=1;
                            fq.insert(k,separSyll);
                        }
                    }
                }
                k = j;
                j -= 2;
                while (j>0 && !signes.contains(fq[j])) j-=1;
            }
            // J'ai placé les césures en suivant les règles établies.
            // Mais je peux avoir des césures étymologiques qui vont à l'encontre des règles "normales".
            QString l_etym = e->getHyphen();
            if (!l_etym.isEmpty()) foreach (QString etym, l_etym.split(','))
            {
                QString fq1 = fq;
                // Je vais parcourir le mot pour vérifier que ça colle
                int i = 0;
                int j = 0;
                int changement = 0;
                bool OK=true;
                while ((i<etym.size()) && (j<fq.size()) && OK)
                {
                    if ((etym[i]==fq[j]) || (fq.mid(j,1)==accentue(etym.mid(i,1))) || ((etym[i] == '-') && (fq[j] == separSyll)))
                    {
                        // Les lettres ou les césures correspondent
                        i+=1;
                        j+=1;
                    }
                    else if (signes.contains(fq[j]) || (fq[j] == 0x0301)) j+=1; // C'est une quantité
                    else if ((etym[i] != '-') && (fq[j] != separSyll)) OK = false; // Les lettres ne correspondent pas.
                    else
                    {
                        // la césure est mal placée.
                        if (etym[i] == '-')
                        {
                            fq.insert(j,"ˌ");
                            changement += 1;
                            j+=1;
                            i+=1;
                        }
                        else
                        {
                            fq.remove(j,1);
                            changement -= 1 ;
                        }
                    }
                }
                if (changement == 1)
                {
                    // La césure étymologique est tombé avant la césure normale...
                    while (fq[j] != separSyll) j+=1;
                    fq.remove(j,1);
                }
                if (!OK) fq = fq1; // etym ne correspondait pas aux premières lettres de fq.
            }
        }
        fq.remove("+");
        fq.remove("-");
        fq.remove("*");
        return fq;
    }
    if (suffixe.isEmpty ()) return fq;
    // les suffixes possibles sont que, ne et ve :
    // tous commencent par une consonne
    return allonge (fq) + suffixe;
}

QString AnalyseMorpho::getForm (int accent)
{
    QString fq = q;
    if (dq.isEmpty ()) // Vient de la liste des irréguliers
        return ajoutSuff(fq, accent);
/*    {
        if (suffixe.isEmpty ()) return fq;
        // les suffixes possibles sont que, ne et ve :
        // tous commencent par une consonne
        return allonge (fq) + suffixe;
    }*/
    if (fq == ".") // Radical vide de sum ou eo
        return ajoutSuff(dq, accent);
/*    {
        if (suffixe.isEmpty ()) return dq;
        return allonge (dq) + suffixe;
    }*/
    // essai de correction pour do :
    if (!e->radicalq (2).isEmpty () && e->radicalq (2)[0].endsWith
        ("d\u0115d") && dq != "\u0101s")
        dq.replace (QRegExp ("^\u0101"), "\u0103");

    // contraction du double i
    if (db_i) dq[0] = 0x012b; // ii -> 'ī';

    // -o des verbes
    if ((QString::compare (dq, "\u014d\u0306") == 0)
        && (QRegExp
            (".*(["+longues+"]["+consonnes+"]?|["+consonnes+"]{2}|[xj])$").exactMatch (q)))
        dq = "\u014d"; // o long

    // collage intelligent fq = fq + dq
    if (consonnes.contains(dq[0]))
        fq = allonge (fq) + dq;
    else
    {
        // fq est un radical prêt à accepter une
        // désinence commençant par une voyelle : le
        // cas de praeest doit passer dans case 22
        fq = fq + dq;
    }
    // puis le suffixe.
    return ajoutSuff(fq, accent);
/*    if (suffixe.isEmpty ()) return fq;
    return allonge (fq) + suffixe;*/
}

void AnalyseMorpho::setUpper ()
{
    q[0] = q[0].toUpper ();
}

void AnalyseMorpho::setLower ()
{
    q[0] = q[0].toLower ();
}

void AnalyseMorpho::setSuff (QString s)
{
    suffixe = s;
}

QString AnalyseMorpho::getSuff ()
{
    return suffixe;
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

Entree * AnalyseMorpho::entree ()
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
