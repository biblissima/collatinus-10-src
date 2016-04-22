/*
 *                          lexicum.cpp
 *                          anciennement libcollatinus.cpp
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
#include <QDebug>

#include "lexicum.h"

/**************************
 *                        *
 *  ESPACE DE NOMMAGE Ch  *
 *                        *
 **************************/

namespace Ch
{
    /**
     * ajoute (QString mot, QStringList liste)
     * ajoute mot au début de chaque item de liste.
     */
    QStringList ajoute (QString mot, QStringList liste)
    {
        QStringList res;
        foreach (QString item, liste)
            res << mot+item;
        return res;
    }
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

    /**
     * @brief deQuant
     * @param c
     * utilisée en cas d'élision
     * supprime la quantité de la voyelle finale de la chaine c
     * lorsque cette voyelle est en fin de mot ou suivie d'un "m".
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
        k.replace (0x1ee5, 'u');  // ụ le u muet de suavis, suadeo, etc...
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
        if (k.contains("["))
            k=k.section("[",0,0)+"`";
        // Je garde une trace de l'élision (pour le rythme)
        k.remove(0x1ee5); // suppression du u-exponctué.
        k.remove(reLettres);
        return k;
    }

    QString versPC (QString k)
    {
        // Comme versPedeCerto, mais ici le mot n'a pas été trouvé.
        // Les voyelles ne sont pas marquées sauf par position...
        k=k.toLower();
        if (k.contains("["))
            k=k.section("[",0,0)+"`";
        k.replace("qu","");
        k.replace("gu","");
        k.replace("āe","+");
        k.replace("ōe","+");
        k.replace("āu","+");
        k.replace("ēu","+");
        // Incomplet : manque la recherche de doubles consonnes ou voyelles
        k.replace("a","*"); k.replace("e","*");
        k.replace("i","*"); k.replace("o","*");
        k.replace("u","*"); k.replace("y","*");
        return versPedeCerto(k);
    }

    void allonge (QString * f)
    {
        *f = AnalyseMorpho::allonge (*f);
    }

    void elide (QString *mp)
    {
        int taille = mp->size ();
        if (taille > 1 && ((mp->endsWith ('m') || mp->endsWith ("\u0101e")) || mp->endsWith ("\u0306"))
            && AnalyseMorpho::voyelles.contains (mp->at (taille - 2)))
        {
            Ch::deQuant (mp);
            mp->insert (taille - 2, '[');
            mp->append (']');
        }
        else if (AnalyseMorpho::voyelles.contains (mp->at (taille - 1)) && *mp !="\u014d")
        {
            Ch::deQuant (mp);
            mp->insert (taille - 1, '[');
            mp->append (']');
        }
    }

    bool estNombre (QString s)
    {
        bool ok=false;
        s.toInt (&ok);
        return ok;
    }

    QStringList decompose (QString ligne)
    {
        QStringList separ;
        int io = ligne.indexOf (Ch::reLettres);
        if (ligne.isEmpty() || (io < 0))
        {
            separ << ligne;
            // C'est une ligne vide ou ne contenant pas de lettre :
            // je la laisse comme elle est !
            return separ;
        }
        if (io > 0)
        {
            separ << ligne.left (io);
            ligne.remove (0,io);
        }
        else separ << "";
        // Maintenant je suis sûr que ligne commence par un mot
        while (ligne.length () > 0)
        {
            io = ligne.indexOf (Ch::reNonLettres);
            // io repère l'indice du premier caractère qui n'est pas une lettre
            if (io < 0)
            {
                // Je suis arrivé à la fin de la ligne
                separ << ligne << "";
                ligne = "";
                continue;
            }
            else
            {
                separ << ligne.left (io);
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
//        if ((separ.size () & 1) == 0) separ << "";
        // La liste contient les mots en positions impaires 1, 3, etc...
        // Les séparateurs sont en positions paires 0, 2, etc...
        // La liste se termine par un séparateur
        // et contient donc un nombre impair d'éléments.
        return separ;
    }

    /**
     * @brief Les deux listes ont-elles un mot en commun ?
     * @param la : première liste de chaines
     * @param lb : deuxième liste de chaines
     * @return true, si au moins un mot est commun. false si les listes sont disjointes
     */
    bool commun (QStringList la, QStringList lb)
    {
        foreach (QString a, la)
            if (lb.contains (a))
                return true;
        return false;
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

} // fin namespace Ch


/*******************
** classe lexique **
********************/
Lexicum::Lexicum (QString rsrcDirName, QObject *parent) :
    QObject(parent)
{
    // lecture du fichier des modèles de radicaux mdlrad.la
    if (rsrcDirName.endsWith("/")) qsuia = rsrcDirName;
    else qsuia = rsrcDirName + "/";
    QString fichier = qsuia + "mdlrad.la";
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
    fichier = qsuia + "desin.la";
    QFile fDes (fichier);
//    QFile fDes ("donnees:desin.la");
    // 23 novembre 2015
    // Je définis un alias avec setSearchFPaths dans main.cpp
    // Ph.
    fDes.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fluxD (&fDes);
    fluxD.setCodec("UTF-8");
    while (!fluxD.atEnd ())
    {
        linea = fluxD.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        Desinence* D = new Desinence (linea, this);
        desinentiae.insert (D->graphie (), D);
        fluxD.flush ();
    }
    fDes.close ();

    // fichier des formes irrégulières irregs.la
    fichier = qsuia + "irregs.la";
    QFile fIrr (fichier);
    fIrr.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream fluxI (&fIrr);
    fluxI.setCodec("UTF-8");
    while (!fluxI.atEnd ())
    {
        linea = fluxI.readLine ();
        if (linea.isEmpty () || linea[0] == '!') continue;
        QString cle = linea.section ('|', 1, 1);
        Entree * e = entree (cle);
        Irreg * I = new Irreg (linea, e, this);
        if (e != NULL) e->ajIrr (I);
#ifdef DEBOG
        else qDebug () << "erreur irr." << linea;
#endif
        irregulares.insert (I->graphie (), I);
        fluxI.flush ();
    }
    fIrr.close ();

    // initialisation des morphos d'adverbes
    //    Traits (int k, int g, int n, int d, int p, int t, int m, int v);
    bitsComp = new Traits (1,3,1,2,0,0,0,0, this); // morpho de l'adj = adv au comparatif
    bitsSup = new Traits (2,1,1,3,0,0,0,0, this);  // au superlatif
    bitsCAdv = new Traits (0,0,0,2,0,0,0,0, this); // morpho de l'adj = adv au comparatif
    bitsSAdv = new Traits (0,0,0,3,0,0,0,0, this);  // au superlatif

    // lecture des fichiers de traduction
    traductionesLege();

    // lecture du fichier des pronoms composés pronomina.la
    pronominaLege ();

    // règles de quantité
    QFile fregles (qsuia +"regles.txt");
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
    // voyelles = "a\u0101\u0103e\u0113\u0115i\u012b\u012do\u014d\u014fu\u016b\u016d";
#ifdef DEBOG
    // Vérifier qu'il n'y a pas d'entrée sans traduction - Ph. 29/8/14
    foreach (Entree * e, entrees.values())
    {
        if (e->n_a_pas_trad())
            qDebug() << e->canonR() << "sans traduction";
    }
#endif
}

Lexicum::~Lexicum ()
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

void Lexicum::traductionesLege(QString nomen)
{
    // lecture des fichiers de traduction
    QDir repertoire = QDir (qsuia, nomen+".*");
    QStringList lemmata = repertoire.entryList ();
    lemmata.removeOne (nomen+".la");
    foreach (QString lem, lemmata)
    {
        QString ext = QFileInfo (lem).suffix ();
        linguamLege (ext, nomen);
        // traduction des entrées à renvoi 'cf.'
        foreach (Entree * el, entreesCf)
        {
            if (el == NULL)
            {
                continue;
            }
            Entree * ebis = renvoi (el);
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
        Entree * el = adjAdv.takeFirst ();
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
} // traductionesLege

void Lexicum::ampliatio()
{
    // Lecture du fichier de lemmes lem_ext.la
    lemmataLege ("lem_ext");

    traductionesLege("lem_ext");

    // lecture du fichier des pronoms composés pronomina.la
//    pronominaLege ("pron_ext");

#ifdef DEBOG
    // Vérifier qu'il n'y a pas d'entrée sans traduction - Ph. 29/8/14
    foreach (Entree * e, entrees.values())
    {
        if (e->n_a_pas_trad())
            qDebug() << e->canonR() << "sans traduction";
    }
#endif
}

QMap<QString,QString> Lexicum::cible ()
{
    return metae;
}

void Lexicum::lemmataLege (QString nomen)
{
    // lit un lemmata.[lang]
    QString fichier = qsuia + nomen + ".la";
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
        Entree *E = new Entree (linea, this);
        // ajout de l'entrée
        entrees[cle] = E;
        if (linea.contains ("@"))
        {
            adjAdv << E;
        }
        if (linea.contains ("cf. ")) entreesCf << E;
        // ajout des irréguliers canoniques
        foreach (Irreg * I, E->irreguliers ())
        {
            irregulares.insert (I->graphie (), I);
        }
#ifdef DEBOG
    // Vérifier qu'il n'y a pas d'erreur de modèle - Ph. 19/4/16
        if (!modelesRad.keys().contains(E->modele()))
            qDebug() << "Erreur de modèle dans " << linea;
#endif
        // ajout des radicaux
        for (int n=1;n<=modelesRad[E->modele ()].count ();++n)
        {
            for (int i = 0; i < E->radicalq (n).size(); ++i)
            {
                QString rq = E->radicalq (n)[i];
                QString r = Ch::deramise (rq);
                if (!r.isEmpty ())
                {
                    // passage de Entree::modeleRad à Lexicum::modelesRad[]
                    radices.insert (r, new Radix (E, modelesRad[E->modele ()][n-1], n, rq, this));
                    // éviter les radicaux de comparatif et superlatif
 /*                   if (rq.endsWith ("ĭ") && (modelesRad[E->modele ()][n]!=11 || n==1))
                    {
                        r.chop (1);
                        rq.chop (1);
                        radices.insert (r, Radix (E, modelesRad[E->modele ()][n-1], n, rq, true));
                    } */
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
 * pronom simple, qui peut se fléchir, dans ce dernier
 * cas, à l'intérieur de la forme : quidam, gén.s. cuiusdam
 */
void Lexicum::pronominaLege (QString nomen)
{
    QFile f (qsuia + nomen + ".la");
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
        Entree * eComp = entree (canon);
        if (eComp == NULL) continue;
        // chercher le simple dans entrees
        QString simple = linea.section ('|', 1, 1);
        Entree * eSimple = entree (simple);
        if (eSimple == NULL) continue;
        // liste des irréguliers excl. de eComp.
        QList<int> irrList;
        foreach (Irreg * ii, eComp->irreguliers ())
            if (ii->exclusif ())
            {
                irrList << 6 * ii->getTraits ()->casus ()
                    + 2 * ii->getTraits ()->genus ()
                    + ii->getTraits ()->numerus ();
            }
        QString affixe = linea.section ('|', 2, 2);
        if (affixe.contains("*"))
        {
        bool dup = (affixe == "**");
        bool suff = (!dup) && affixe.startsWith ('*');
        affixe.remove ('*');
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
                        if (dup) fs = AnalyseMorpho::allonge (fs) + fs;
                        else if (suff) fs = AnalyseMorpho::allonge (fs) + affixe;
                        else fs = affixe + fs;
                        QString l = genl.arg (fs).arg (canon).arg (c).arg(g).arg(n); 
                        Irreg * irr = new Irreg (l, eComp, this);
                        eComp->ajIrr (irr);
                        irregulares.insert (Ch::deramise (irr->graphie ()), irr);
                    }
                }
        }
        else
        {
            // Je dois faire la même chose pour les noms, avec une boucle en moins.
            // Deux cas possibles, paterfamilias et respublica, codés en #familias et ##
            bool dup = (affixe == "##");
            affixe.remove ('#');
            if (dup) genl = QString ("%1*|%2|%3|%4|%5|0|0|0|0|0");
            // Comme je décline le composé pour avoir la bonne fin, je mets le résultat en exclusif
            for (int c=1;c<7;++c)
                    for (int n=1;n<3;++n)
                    {
                        // abandon en cas de fc ou de présence d'un irrég exclusif
                        if ((c==1 && n==1) || irrList.contains (6*c + n))
                            continue;
                        QStringList fSimple = synthN (eSimple, c, n);
                        if (fSimple.empty ()) continue ;
                        foreach (QString fs, fSimple)
                        {
                            if (dup)
                            {
                                // pour respublica
                                QStringList fComp = synthN (eComp, c, n);
                                if (fComp.empty ()) continue ;
                                QString fc = fComp [0].remove(0,simple.size());
                                fs = AnalyseMorpho::allonge (fs) + fc;
                            }
                            else fs = AnalyseMorpho::allonge (fs) + affixe;
                            QString l = genl.arg (fs).arg (canon).arg (c).arg(0).arg(n);
                            Irreg * irr = new Irreg (l, eComp, this);
                            eComp->ajIrr (irr);
                            irregulares.insert (Ch::deramise (irr->graphie ()), irr);
                        }
                    }
        }
    }
}

void Lexicum::linguamLege (QString lang, QString nomen)
{
    QString fichier = qsuia + nomen + "." + lang;
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
        Entree * e = entree (Ch::jviu (ecl[0]));
        if (e!=NULL)
            e->ajTrad (lang, ecl[1]);
#ifdef DEBOG
        else qDebug () << linea << "erreur linguamLege" << lang;
#endif
    }
    Capsa.close ();
    if (nomen == "lemmata") metae[lang] = linguaTr;
    // Ne remplir la liste des langues cibles que pour les lemmata.*
    // Pour les extensions, ce sont les même langues (Français et English)
}

void Lexicum::dicLinguam (QString l)
{
    if (metae.keys().contains(l))
        lingua = l; // La langue existe.
}

void Lexicum::changeMajPert (bool m)
{
    MajPert = m;
}

QString Lexicum::lang ()
{
    return lingua;
}

/*
 * Ambrogio () traduit le mot latin dans
 * toutes les langues cibles disponibles
 */
QString Lexicum::ambrogio (Entree * e)
{
    return e->ambrogio ();
}

/**
 * Renvoi d'entrée.
 * Détecte si l'entrée pointe sur une autre entrée,
 * et dans ce cas renvoie cette autre entrée.
 * sinon, renvoie la même entrée
 */
Entree *Lexicum::renvoi (Entree *e)
{
    QString r = e->getCf ();
    if (r.isEmpty ()) return e;
    return entree (r);
}

Entree * Lexicum::entree (QString k)
{
#ifdef DEBOG
    if (entrees.count (k) == 0)
    {
        qDebug () << k << "entrée inexistante.";
        return NULL;
    }
    return entrees[k];
#else
    Entree * e = entrees[k];
    if (e == NULL)
    {
        return NULL;
    }
    return e;
#endif
}

QString Lexicum::radical (QString r)
{
    QString sortie;
    QTextStream ts (&sortie);
    foreach (Radix * radix, radices.values (r))
        ts << radix->doc () << "\\n";
    return sortie;
}

void Lexicum::lajoute (QString l, ListeAnalyses& la, AnalyseMorpho * am)
{
    la.insert (l, am);
    // ajout éventuel de l'adverbe
    if (am->entree ()->getAdv ().count () > 0)
    {
        QString f = am->getQ ();
        QString dd;
        foreach (Entree * e, am->entree ()->getAdv ())
        {
            if (e==NULL) continue;
            Traits * t;
            if (am->getTraits ()->contient (bitsComp))
            {
                t = bitsCAdv;
                dd = am->getD ();
            }
            else if (am->getTraits ()->contient (bitsSup))
            {
                t = bitsSAdv;
                dd = am->getD ();
                dd.chop (1); dd.append (0x0113);
            }
            else t = NULL;
            if (t!= NULL)
            {
                AnalyseMorpho * amadv = new AnalyseMorpho (f, t, e, dd, this);
                QString def = e->definition (lingua);
                la.insert (def, amadv);
            }
        }
    }
}

/**
 * Lexicum::lanalyses utilise une sortie sous
 * forme de liste d'analyses, dont tous
 * les items sont des instances de la
 * classe AnalyseMorpho. 
 * Lexicum::lanalyses supervise les recherches faites
 * par corAnalyses en une chaine d'appels successifs.
 * Il gère une éventuelle majuscule initiale en tenant
 * compte de l'option MajPert, et appelle
 * Lexicum::lanalysesE.
 * Qui cherche un possible enclitique, et appelle
 * Lexicum::lanalysesA qui fait enfin l'assimilation
 * des préfixes. Chaque transformation est reproduite
 * sur la forme avec quantités. Les appels sucessifs
 * permettent les combinaisons de toutes les
 * transformations : par exemple, Adclamatioque
 */

ListeAnalyses Lexicum::lanalyses (QString forme, bool deb_phr)
{
    if (forme.size() == 0) return lanalysesE (forme);
    if ((forme.at (0) == 'V') && !(forme.contains ('v')))
        forme[0] = 'U';
    ListeAnalyses la = lanalysesE (forme); // passage direct à la détection d'enclitiques
    if (MajPert && !deb_phr && !la.isEmpty()) return la;
    // Les majuscules sont pertinentes, je ne suis pas en début de phrase et j'ai une solution :
    // je m'arrête là !

    // Majuscule initiale
    if (forme.at (0).isUpper ())
    {
        ListeAnalyses adde = lanalysesE (forme.toLower ()); // Pour traiter les enclitiques et les assimilations
        foreach (AnalyseMorpho * am, adde.values ())
        {
            am->setUpper ();
        }
        la += adde;
    }
    if (MajPert && !la.isEmpty()) return la;
    // La majuscule en début de phrase est le seul cas accepté si les maj. sont pertinentes

    // En désespoir de cause
    if (forme.at (0).isLower ())
    {
        forme[0] = forme.at(0).toUpper();
        ListeAnalyses adde = lanalysesE (forme);
        foreach (AnalyseMorpho * am, adde.values ())
        {
            am->setLower ();
        }
        la += adde;
    }
    return la;
}

/* Traitement des suffixes */
ListeAnalyses Lexicum::lanalysesE (QString forme)
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
        foreach (AnalyseMorpho * am, adde.values ())
        {
            am->setSuff (suffixe); 
        }
        la += adde;
    }
    return la;
}

/* Traitement des assimilations */
ListeAnalyses Lexicum::lanalysesA (QString forme)
{
    ListeAnalyses la = corAnalyses (forme); // passage direct au cœur du moteur d'analyses
    int nAss = 0;
    QString fpr = AnalyseMorpho::assimile (forme, nAss);
    if (fpr != forme)
    {
        ListeAnalyses adde = corAnalyses (fpr); // Attention ! pas de récursion ici, sinon boucle infinie.
        if (!adde.isEmpty())
        {
            foreach (AnalyseMorpho * am, adde.values ())
            {
                am->assimileq (nAss);
            }
            la += adde;
        }
    }
    return la;
}

/**
 * cor (lat cor, cordis, angl core)
 * le cœur du moteur d'analyses
 *
 * Lexicum::corAnalyses cherche
 * la forme qui lui est donnée dans les listes
 * des formes canoniques, puis en la
 * décomposant en radical+désinence.
 * Il la cherche enfin dans la liste des irréguliers.
 * Il retourne une liste d'analyses
 * qui est gérée par le dispositif d'appels successifs
 * lanalyses - lanalysesE - lanalysesA
 */

ListeAnalyses Lexicum::corAnalyses (QString forme)
{
    int cnt = forme.toLower().count("v");
    int cnt_ae = forme.toLower().count("æ");
    int cnt_oe = forme.toLower().count("œ");
    if (forme.toLower().endsWith("æ")) cnt_ae -= 1;
    // ramismes
    forme = Ch::jviu (forme);
    forme.replace("æ","ae");
    forme.replace("Æ","Ae");
    forme.replace("œ","oe");
    forme.replace("Œ","Oe");
    // variables
    ListeAnalyses la;
    lr liste_r;
//    lr::iterator ilr;
    Entree * e;
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
        QList<Desinence*> liste_d = desinentiae.values (d);
        if (liste_d.empty ()) continue;
        // d correspond à (au moins) une désinence

        // recherche et mise en liste des radicaux
        liste_r.clear ();
        // Traiter d'abord les cas où r est un radical incomplet
        // la désinence est-elle V_phage ?
        // pour traiter la suppression des -ve- ou -vi- dans certaines formes dérivées du parfait
//        bool supp_v = liste_d[0]->est_V_phage() && (r.endsWith("a") || r.endsWith("e") || r.endsWith("no"));
        bool supp_v = liste_d[0]->est_V_phage();
        if (supp_v)
        {
            foreach (Radix * radix, radices.values (r+"u"))
                if (((cnt==0)||(cnt + 1 == radix->grquant().toLower().count("v")))
                        && ((cnt_oe==0)||(cnt_oe == radix->grquant().toLower().count("ōe")))
                        && ((cnt_ae==0)||(cnt_ae == radix->grquant().toLower().count("āe"))))
                    liste_r.push_back (radix);
        }
        else
        {
            if (d.startsWith ("i") && !r.endsWith("i"))
            {
                // Y a-t-il une contraction d'un double i ?
                foreach (Radix * radix, radices.values (r+"i"))
                    if (((cnt==0)||(cnt == radix->grquant().toLower().count("v")))
                            && ((cnt_oe==0)||(cnt_oe == radix->grquant().toLower().count("ōe")))
                            && ((cnt_ae==0)||(cnt_ae == radix->grquant().toLower().count("āe"))))
                        liste_r.push_back (radix);
            }
            foreach (Radix * radix, radices.values (r))
                if (((cnt==0)||(cnt == radix->grquant().toLower().count("v")))
                        && ((cnt_oe==0)||(cnt_oe == radix->grquant().toLower().count("ōe")))
                        && ((cnt_ae==0)||(cnt_ae == radix->grquant().toLower().count("āe"))))
                    liste_r.push_back (radix);
        }
/*        qDebug() << r << d << supp_v << liste_r.size();
        if (liste_r.size()>0) foreach (Radix rad, liste_r) {
            qDebug () << rad.grquant();
        } */
/*        foreach (Radix radix, radices.values (r))
            if ((!radix.en_i () || d.startsWith ("i")) && ((cnt==0)||(cnt + cnt_v == radix.grquant().toLower().count("v"))))
                liste_r.push_back (radix); */
        // recherche des désinences
        QString radq;
        foreach (Desinence * des, liste_d)
        {
            // boucle de recherche dans les radicaux
//            ilr = liste_r.begin ();
//            while (ilr != liste_r.end ())
            foreach (Radix * radix, liste_r)
            {
                // comparaison du modèle et du n° de radical
//                if ((des->mdl ().contains (ilr->mdl ())) && (ilr->nr () == des->nr ()))
                if ((des->mdl ().contains (radix->mdl ())) && (radix->nr () == des->nr ()))
                {
//                    e = ilr->entree ();
                    e = radix->entree ();
                    if (e != NULL)
                    {
                        if (!((e->getNombre () == 2) && (des->getTraits ()->numerus () == 1)))
                        {
//                            radq = ilr->grquant ();
                            radq = radix->grquant ();
                            bool supp_i = !r.endsWith("i") && radq.endsWith("ĭ");
                            // Contrairement à supp_v qui ne dépend que de la désinence, supp_i dépend aussi du radical.
                            // En effet, "Juli" peut être le génitif normal de Julus ou celui contracté de Julius.
                            if (supp_v || supp_i) radq.chop(1);
                            bool fauxReg = false;
                            foreach (Irreg * ir, e->irreguliers ())
                            {
                                fauxReg = fauxReg || (ir->exclusif () && ir->homomorphe (des));
                            }
                            // éliminer les réguliers homomorphes d'un irrégulier exclusif
                            if (!fauxReg)
                            {
                                //en_i, ii = ī : allonger l'initiale de la désinence.
                                AnalyseMorpho * anMo = new AnalyseMorpho (radq, des, e, "", supp_i, this);
//                                AnalyseMorpho * anMo = new AnalyseMorpho (radq, des, e, "", ilr->en_i ());
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
//                ++ilr;
            }
        }
    } // fin de boucle de recherche r.d
    foreach (Irreg * irr, irregulares.values (forme))
    {
        e = irr->entree ();
        AnalyseMorpho * am = new AnalyseMorpho (irr->graphieq (), irr->getTraits (), e, "", this);
            lajoute (e->definition (lingua), la, am);
    }
    return la;
}

void Lexicum::deleteAnalyses (ListeAnalyses la)
{
    foreach (QString cle, la.keys ())
        la.remove (cle);
}

QStringList Lexicum::lemmatiseM (QString f, bool mm, bool deb_phr)
{
    ListeAnalyses r = lanalyses (f, deb_phr);
    QStringList sortie;
    // les romains, en amont parce que les romains ne sont
    // pas dans le lexique.
    if (Ch::est_romain (f))
        sortie << QString ("%1 : %2").arg (f).arg (Ch::aRomano (f));
    // minuscules pour toutes les lettres sauf la première
//    Ch::minuscules (f);
    // récup. des analyses
    if (r.empty () && sortie.isEmpty ())
    {
        return QStringList () << f;
    }
    // rassembler les clés
    QStringList cles = r.keys ();
    cles.removeDuplicates ();
    /* anciennement
    QStringList cles;
    foreach (QString cle, r.keys ())
    {
       if (!cles.contains (cle)) cles << cle;
    }
    */
    foreach (QString cle, cles)
    {
        QString lin = cle;
        if (mm)
        {
            foreach (AnalyseMorpho * am, r.values (cle))
                lin.append ( "<br/>\n&nbsp;&nbsp;" + am->humain ());
        }
        sortie << lin;
    }
    deleteAnalyses (r);
    return sortie;
}

void Lexicum::verbaCognita(QString fichier,bool vb)
{
    if (vb)
    {
        hLem.clear();
        couleurs.clear();
        // Couleurs par défaut
        couleurs << "#00A000"; // vert
        couleurs << "#000000"; // noir
        couleurs << "#A00000"; // rouge
        if (!fichier.isEmpty())
        {
            QFile file(fichier);
            if (!file.open(QFile::ReadOnly | QFile::Text))
            {
                nbErreurs += 1;
                msgErreur.append(tr("Capsam legere nequeo %1:\n%2.\n")
                                 .arg(fichier)
                                 .arg(file.errorString()));
/*                QMessageBox::warning(0, tr("Collatinus"),
                                     tr("Capsam legere nequeo %1:\n%2.")
                                     .arg(fichier)
                                     .arg(file.errorString())); */
                return;
            }

            QTextStream in(&file);
            QString ligne = in.readLine();
            while (ligne.startsWith("!") || ligne.isEmpty()) ligne = in.readLine();
            // Je saute les commentaires et les lignes vides.
            int i = 0;
            while (ligne.startsWith("#") &&  !in.atEnd())
            {
                if ((i<3) && (ligne.size() == 7)) couleurs[i] = ligne;
                i+=1;
                ligne = in.readLine();
            }
            // Je peux changer les couleurs dans le fichier
            QStringList item;
            while (!in.atEnd())
            {
                if (!ligne.startsWith("!") && !ligne.isEmpty()) // hLem.insert(ligne,1);
                {
                    item = lemmatiseM (ligne, false, false);
                    if (item.at(0) == ligne) hLem.insert(ligne,1);
                    else
                    {
                        for (int jj=0; jj<item.size();jj++)
                        {
                            QString lem = Ch::ote_diacritiques(item[jj].left(item[jj].indexOf(",")));
                            lem.replace("j","i");
                            lem.replace("J","I");
                            lem.remove("<strong>");
                            lem.remove("</strong>");
                            hLem.insert(lem,1);
                        }
                    }
                }
                ligne = in.readLine();
            }
        }
    }
    else
    {
        hLem.clear();
        couleurs.clear();
    }
}

QString Lexicum::lemmatiseTxt (QString &txt, bool alpha, bool cumVocibus, bool cumMorpho)
{
    if (txt.isEmpty()) return "";
    // benchmark 1ère mesure
    // qint64 tDebut = QDateTime::currentDateTime ().toMSecsSinceEpoch ();
    bool deb_phr/*, deb_phr_suiv*/ = true;
//    QStringList formes = txt.split (Ch::reEspace, QString::SkipEmptyParts);
    QStringList formes = txt.split (Ch::reLimitesMots);
//    QStringList formes = Ch::decompose (txt);
    QStringList lemmes;
    QStringList echecs;
    QStringList item;
    QString uox;
    bool cumColoribus = !couleurs.isEmpty();
    bool listeVide = hLem.isEmpty();
    int colPrec = 0;
    int formesConnues = 0;
//    foreach (QString uox, formes)
    for (int i=1;i<formes.length();i+=2)
    {
        uox = formes [i];
        deb_phr = formes [i-1].contains (Ch::rePonct) || i == 1;
        item = lemmatiseM (uox, cumMorpho, deb_phr);
        if (item.at (0) == uox)
        {
            if (alpha) echecs << item;
            else lemmes << QString ("</div><span style=\"color:blue;\">%1</span><div style=\"margin-left:20px\">").arg (item.at (0));
            if (cumColoribus)
            {
                if (!listeVide)
                {
                    // La liste de mots connus n'est pas vide. Le mot en fait-il partie ?
                    QString lem = uox;
                    lem.replace("j","i");
                    lem.replace("J","I");
                    // qDebug() << lem;
                    if (hLem.contains(lem))
                    {
                        if (colPrec != 0)
                        {
                            formes[i].prepend("</span><span style=\"color:"+couleurs[0]+"\">");
                            colPrec = 0;
                        }
                    }
                    else
                    {
                        if (colPrec != 2)
                        {
                            formes[i].prepend("</span><span style=\"color:"+couleurs[2]+"\">");
                            colPrec = 2;
                        }
                    }
                }
            }
        }
        else
        {
            bool connu = false;
            if (cumColoribus)
            {
                if (!listeVide)
                {
                    // La liste de mots connus n'est pas vide. Un des lemmes identifiés en fait-il partie ?
                    for (int jj=0; jj<item.size();jj++)
                    {
                        QString lem = Ch::ote_diacritiques(item[jj].left(item[jj].indexOf(",")));
                        lem.replace("j","i");
                        lem.replace("J","I");
                        lem.remove("<strong>");
                        lem.remove("</strong>");
                        connu = connu || hLem.contains(lem);
                    }
                }
                if (connu)
                {
                    formesConnues += 1;
                    if (colPrec != 0)
                    {
                        formes[i].prepend("</span><span style=\"color:"+couleurs[0]+"\">");
                        colPrec = 0;
                    }
                }
                else if (colPrec != 1)
                {
                    formes[i].prepend("</span><span style=\"color:"+couleurs[1]+"\">");
                    colPrec = 1;
                }
            }
            if (cumVocibus)
                item[0].insert (0, "</div><strong><em>"+uox+"</em></strong>\n<div style=\"margin-left:20px\">");
            if (!connu || listeVide) lemmes << item.join ("<br/>\n");
            // Par défaut, pas d'aide pour les mots connus.
        }
    }
    if (alpha)
    {
        lemmes.removeDuplicates ();
        qSort (lemmes.begin (), lemmes.end (), Ch::sort_i);
    }
    if (cumVocibus)
    {
         lemmes[0].remove(0,6);
         lemmes << "</div>";
    }
    if (echecs.size () > 0)
    {
        echecs.removeDuplicates ();
        int nbEchecs = echecs.size ();
        double somme = echecs.size () + lemmes.size ();
        lemmes << QString ("<strong>HAEC %1 / %2 NON RECOGNITA (%L3 %):</strong>\n")
            .arg (nbEchecs)
            .arg (somme)
            .arg (QString::number ((nbEchecs / somme * 100), 'g', 4));
        if (alpha)
            qSort (echecs.begin (), echecs.end (), Ch::sort_i);
        lemmes << echecs;
    }
    if (cumColoribus)
    {
        formes[0].append("<span style=\"color:"+couleurs[0]+"\">");
        formes[formes.size()-1].append("</span>");
        txt = formes.join("");
        txt.replace("\n","<br/>\n");
        if (!listeVide)
        {
            QString stats = "<strong>Formes connues : %1 sur %2 (%3%)<br/></strong>";
            lemmes.prepend(stats.arg(formesConnues).arg((formes.size()/2)).arg((200*formesConnues)/formes.size()));
        }
    }

    // benchmark 2ème mesure
    // qDebug () << QDateTime::currentDateTime ().toMSecsSinceEpoch () - tDebut;
    return lemmes.join ("<br/>\n");
}

QString Lexicum::par_position (QString f)
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
QStringList Lexicum::synthA (Entree* e, int c, int n, int g, int d)
{
    // chercher dans les irréguliers
    QStringList retour;
    foreach (Irreg * ii, e->irreguliers ())
    if (        ii->getTraits ()->casus () == c
                && ii->getTraits ()->numerus () == n
                && ii->getTraits ()->genus () == g
                && ii->getTraits ()->gradus () == d)
        {
            retour << ii->graphieq ();
            if (ii->exclusif ())
                return retour;
        }
    foreach (Desinence* des, desinentiae)
    {
        if ((modelesRad[e->modele()].size() >= des->nr())
            && (des->mdl ().contains (modelesRad[e->modele()][des->nr ()-1]))
            && des->getTraits ()->casus () == c
            && des->getTraits ()->numerus () == n
            && des->getTraits ()->genus () == g
            && des->getTraits ()->gradus () == d)
        {
            foreach (QString r, e->radicalq (des->nr ()))
            if (AnalyseMorpho::consonnes.contains(des->grquant ()[0]))
                retour << AnalyseMorpho::allonge (r) + des->grquant ();
            else retour << r + des->grquant ();
        }
    }
    return retour;
}

/**
 * Synthèse des formes nominales
 */
QStringList Lexicum::synthN (Entree *e, int c, int n)
{
    // chercher dans les irréguliers
    QStringList retour;
    foreach (Irreg * ii, e->irreguliers ())
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
    foreach (Desinence* des, desinentiae.values ())
    {
        if ((modelesRad[e->modele()].size() >= des->nr())
            && (des->mdl ().contains (modelesRad[e->modele()][des->nr ()-1]))
            && des->getTraits ()->casus () == c
            && des->getTraits ()->numerus () == n)
        {
            foreach (QString r, e->radicalq (des->nr ()))
            {
                if (AnalyseMorpho::consonnes.contains(des->grquant ()[0]))
                    retour << AnalyseMorpho::allonge (r) + des->grquant ();
                else retour << r + des->grquant ();
            }
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
 * Synthèse des formes pronominales
 */
QStringList Lexicum::synthP (Entree* e, int c, int n, int g)
{
    // éliminer les sing. et pl. inexistants
    // chercher dans les irréguliers
    QStringList retour;
    if (((!e->aUnPluriel ()) && n==2) || ((!e->aUnSingulier ()) && n==1))
    {
        retour << "-";
        return retour;
    }
    foreach (Irreg * ii, e->irreguliers ())
    if (        ii->getTraits ()->casus () == c
                && ii->getTraits ()->numerus () == n
                && ii->getTraits ()->genus () == g)
        {
            retour << ii->graphieq ();
            if (ii->exclusif ())
                return retour;
        }
    foreach (Desinence* des, desinentiae.values ())
        if ((modelesRad[e->modele()].size() >= des->nr())
            && (des->mdl ().contains (modelesRad[e->modele()][des->nr ()-1]))
            && des->getTraits ()->casus () == c
            && des->getTraits ()->numerus () == n
            && des->getTraits ()->genus () == g)
        {
            foreach (QString r, e->radicalq (des->nr ()))
            if (AnalyseMorpho::consonnes.contains(des->grquant ()[0]))
                retour << AnalyseMorpho::allonge (r) + des->grquant ();
            else retour << r + des->grquant ();
        }
    return retour;
}

/**
 * Synthèse des formes verbales
 */
QStringList Lexicum::synthV (Entree* e, int p, int n, int t, int m, int v)
{
    QStringList retours;
    // chercher dans les irréguliers
    bool exclusif = false;
    foreach (Irreg * ii, e->irreguliers ())
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
        foreach (Desinence* des, desinentiae.values ())
        {
            if (
                    des->mdl ().contains (22) // modèle de sum
                    && des->getTraits ()->persona () == p
                    && des->getTraits ()->numerus () == n
                    && des->getTraits ()->tempus () == t
                    && des->getTraits ()->modus () == m
               )
            {
                lsum << des->grquant ();
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
        QList<Desinence*> ldes;
        foreach (Desinence* des, desinentiae.values ())
            if ((modelesRad[e->modele()].size() >= des->nr())
                && des->mdl ().contains (modelesRad[e->modele()][des->nr()-1])
                && des->getTraits ()->persona () == p
                && des->getTraits ()->numerus () == n
                && des->getTraits ()->tempus () == t
                && des->getTraits ()->modus () == m
                && des->getTraits ()->uox () == v
               )
            {
                ldes.append (des);
            }
        for (int j=0;j<ldes.length();++j)
        {
            QStringList lr = e->radicalq (ldes[j]->nr());
            if (lr.empty () || lr[0].isEmpty ())
                retours.append("-");
            else for (int i=0;i<lr.length();++i) // radicaux multiples séparés par ','
            {
                if (lr[i] == ".") lr[i] = "";
                QString r = lr[i];
                if (ldes[j]->est_V_phage())
                {
                    if (r.endsWith("āv") || r.endsWith("ēv") || r.endsWith("nōv"))
                    {
                        // pour l'instant, mieux vaut ne pas afficher toutes les formes abrégées
                        // r.chop(1);
                        // retours << r + ldes[j]->grquant ();
                    }
                }
 /*               else if (r.endsWith("ĭ") && ldes[j]->graphie().startsWith("i"))
                {
                    QString dd = ldes[j]->grquant ();
                    retours << r + dd;
                    r.chop(1);
                    dd = "ī" + dd.remove(0,1);
                    retours << r + dd; // à commenter si on ne veut pas afficher les ĭi -> ī
                } */
                else if (AnalyseMorpho::consonnes.contains(ldes[j]->grquant ()[0]))
                    retours << AnalyseMorpho::allonge (r) + ldes[j]->grquant ();
                else retours << r + ldes[j]->grquant ();
            }
        }

    }
    return retours;
}

/**
 * Synthèse des formes participiales
 */
QStringList Lexicum::synthPart (Entree* e, int c, int g, int n, int t, int v)
{
    QStringList retour;
    foreach (Desinence* des, desinentiae.values ())
        if ((modelesRad[e->modele()].size() >= des->nr())
            && des->mdl ().contains (modelesRad[e->modele()][des->nr ()-1])
            && des->getTraits ()->casus () == c
            && des->getTraits ()->numerus () == n
            && des->getTraits ()->genus () == g
            && des->getTraits ()->tempus () == t
            && des->getTraits ()->modus () == 5
            && (des->getTraits ()->uox () == v)
            )
        {
            foreach (QString r, e->radicalq (des->nr ()))
            {
                if (r.isEmpty ())
                    continue;
                if (r == ".")
                    retour << des->grquant ();
                else retour << r + des->grquant ();
            }
        }
    return retour;
}

QString Lexicum::flechis (Entree* e)
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
                    ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << synthN (e, ic, 1).join (",") << "</td></tr>\n";
                }
            ss << "<tr><td><strong>plur.</strong></td></tr>\n";
            if (e->aUnPluriel ())
                for (ic=1;ic < 7;++ic)
                {
                    ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << synthN (e, ic, 2).join (",") << "</td></tr>\n";
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
                ss << "<tr><td><strong>" << Traits::degres[id].toUpper() << "</strong></td></ŧr>" << endl;
                int in = 1;
                if (e->getNombre () == 2) in=2; // pour tenir compte de "pl."
                for (; in<3; ++in)
                {
                    ss << "<tr><td><strong>" << Traits::nombres[in] << "</strong></td><td>M.</td><td>F.</td><td>N.</td></ŧr>" << endl;
                    for (int ic = 1;ic < 7;++ic)
                    {
                        // synthA (cas, nombre, genre, degré)
                        chM = synthA (e, ic,in,1,iid).join (",");
                        chF = synthA (e, ic,in,2,iid).join (",");
                        chN = synthA (e, ic,in,3,iid).join (",");
                        ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
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
                    ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
                }
            ss << "<tr><td><strong>plur.</strong></td></td><td>M.</td><td>F.</td><td>N.</td></tr>";
            if (e->aUnPluriel ())
                for (ic=1;ic<7;++ic)
                {
                    chM = synthP (e, ic, 2, 1).join (",");
                    chF = synthP (e, ic, 2, 2).join (",");
                    chN = synthP (e, ic, 2, 3).join (",");
                    ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << chM << "</td><td>" << chF << "</td><td>"<< chN << "</td></tr>";
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
                    ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << synthV (e, ip,in,1,1,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,2,1,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,3,1,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong> pf.</strong></td><td><strong>fut. ant.</strong></td><td><strong>pqp.</strong></td></tr>";
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" <<  synthV (e, ip,in,4,1,1).join (",") << "</td>";
                    ss << "<td>" <<  synthV (e, ip,in,5,1,1).join (",") << "</td>";
                    ss << "<td>" <<  synthV (e, ip,in,6,1,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong>coniunctiuus</strong></td></tr>" << endl;
            ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>impf.</strong></td></tr>" << endl;
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << synthV (e, ip,in,1,2,1).join (",") << "</td>";
                    ss << "<td>" << synthV (e, ip,in,3,2,1).join (",") << "</td></tr>";
                }
            ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>pqp.</strong></td></tr>" << endl;
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << synthV (e, ip,in,4,2,1).join (",") << "</td>";
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
                        ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
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
                    ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                }
                if (in == 1) ss << "<tr><td><strong>plur.</strong></td></tr>" << endl;
            }
            if (e->est_deponent () || e->modele () == 22 || e->est_intransitif () || e->modele () == 36)
                ss << "<tr><td colspan=\"3\">deponentibus non passiua uox, neque uerbo sum, neque uerbo facio, neque intransitiuibus.</td></tr>" << endl;
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
                        ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>fut. ant.</strong></td><td><strong>pqp</strong></td></tr>";
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,4,1,2).join (",");
                        col2 = synthV (e, ip,in,5,1,2).join (",");
                        col3 = synthV (e, ip,in,6,1,2).join (",");
                        ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>coniunctiuus</strong></td></tr>" << endl;
                ss << "<tr><td></td><td><strong>praes.</strong></td><td><strong>impf.</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,1,2,2).join (",");
                        col2 = synthV (e, ip,in,3,2,2).join (",");
                        ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td></tr>";
                    }
                ss << "<tr><td></td><td><strong>pf.</strong></td><td><strong>pqp</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        col1 = synthV (e, ip,in,4,2,2).join (",");
                        col2 = synthV (e, ip,in,6,2,2).join (",");
                        ss << "<tr><td>" << Traits::personnes[ip] << "</td><td>" << col1 << "</td><td>" << col2 << "</td></tr>";
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
                        ss << "<tr><td>" << Traits::cass[ic] << "</td><td>" << col1 << "</td><td>" << col2 << "</td><td>" << col3 << "</td></tr>";
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

QString Lexicum::ListeFlechie (Entree* e, QString clef)
{
    clef = "," + clef +"\n";
    int ic;
    QStringList ss;
    switch (e->pars ())
    {
        case 'n':
        {
            if (e->aUnSingulier ())
                for (ic=1;ic<7;++ic)
                {
                    ss << synthN (e, ic, 1);
                }
            if (e->aUnPluriel ())
                for (ic=1;ic < 7;++ic)
                {
                    ss << synthN (e, ic, 2);
                }
            ss.removeDuplicates();
            ss.sort();
            return ss.join(clef)+clef;
        }
    case 'a':
        {
            int iid;
            for (int id=1; id<4; ++id)
            {
                if (id==1) iid=0;
                else
                {
                    iid=id;
                    if (e->radicalq(id).empty()) continue; // S'il n'y a pas de comp. ou de sup. on saute.
                }
                int in = 1;
                if (e->getNombre () == 2) in=2; // pour tenir compte de "pl."
                for (; in<3; ++in)
                {
                    for (int ic = 1;ic < 7;++ic)
                    {
                        // synthA (cas, nombre, genre, degré)
                        ss << synthA (e, ic,in,1,iid);
                        ss << synthA (e, ic,in,2,iid);
                        ss << synthA (e, ic,in,3,iid);
                    }
                    if (e->getNombre () == 1) break;
                }
            }
            ss.removeDuplicates();
            ss.sort();
            return ss.join(clef)+clef;
        }
        case 'p':
        {
            int ic = 1;
            if (e->aUnSingulier ())
                for (;ic<7;++ic)
                {
                    ss << synthP (e, ic, 1, 1);
                    ss << synthP (e, ic, 1, 2);
                    ss << synthP (e, ic, 1, 3);
                }
            if (e->aUnPluriel ())
                for (ic=1;ic<7;++ic)
                {
                    ss << synthP (e, ic, 2, 1);
                    ss << synthP (e, ic, 2, 2);
                    ss << synthP (e, ic, 2, 3);
                }
            ss.removeDuplicates();
            ss.sort();
            return ss.join(clef)+clef;
        }
        case 'v':
        {
            int ip, in, it;
            for (in=1;in<3;in++)
                  for (ip=1;ip<4;ip++)
                      for (it=1;it<7;it++)
                            ss << synthV (e, ip,in,it,1,1);
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << synthV (e, ip,in,1,2,1);
                    ss << synthV (e, ip,in,3,2,1);
                }
            for (in=1;in<3;in++)
                for (ip=1;ip<4;ip++)
                {
                    ss << synthV (e, ip,in,4,2,1);
                    ss << synthV (e, ip,in,6,2,1);
                }
            ss << synthV (e,2,1,1,3,1);
            ss << synthV (e,2,1,2,3,1);
            ss <<  synthV (e,2,2,1,3,1);
            ss <<  synthV (e,2,2,2,3,1);
            ss <<  synthV (e,0,0,1,4,1) << synthV (e,0,0,4,4,1);
                int ic;
                for (in=1;in<3;in++)
                {
                    for (ic=1;ic<7;++ic)
                        for (it=1;it<3;it++)
                    {
                        ss << synthPart (e,ic,1,in,it,1);
                        ss << synthPart (e,ic,2,in,it,1);
                        ss << synthPart (e,ic,3,in,it,1);
                    }
                }
            if (!e->est_deponent () && e->modele ()!= 22 && !e->est_intransitif () && e->modele () != 36)
            {
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        ss << synthV (e, ip,in,1,1,2);
                        ss << synthV (e, ip,in,2,1,2);
                        ss << synthV (e, ip,in,3,1,2);
                    }
                for (in=1;in<3;in++)
                    for (ip=1;ip<4;ip++)
                    {
                        ss << synthV (e, ip,in,1,2,2);
                        ss << synthV (e, ip,in,3,2,2);
                    }
                ss << synthV (e,2,1,1,3,2);
                ss << synthV (e,2,2,1,3,2);
                ss << synthV (e,0,0,1,4,2);
//                ss << "<tr><td></td><td><strong>part. pf.</strong></td></tr>" << endl;
                for (in=1;in<3;in++)
                {
                    for (ic=1;ic<7;++ic)
                    {
                        ss << synthPart (e,ic,1,in,4,2).join (",");
                        ss << synthPart (e,ic,2,in,4,2).join (",");
                        ss << synthPart (e,ic,3,in,4,2).join (",");
                    }
                }
            }
            ss.removeDuplicates();
            ss.sort();
            return ss.join(clef)+clef;
        }
        case 'i':
        {
            return e->quantite()+clef;
        }
        default:
            return "";
    }
}

QString Lexicum::flechis(QString clef)
{
    QString cle = Ch::jviu(clef);
    QString sortie = "";
    if (entrees.value(cle) != NULL)
        sortie = ListeFlechie(entrees.value(cle), clef);
    if (entrees.value(cle+"2") != NULL)
        sortie += ListeFlechie(entrees.value(cle+"2"), clef+"2");
    if (entrees.value(cle+"3") != NULL)
        sortie += ListeFlechie(entrees.value(cle+"3"), clef+"3");
    return Ch::ote_diacritiques(sortie);
}

// scansion

QStringList Lexicum::formeq (QString forme, bool *nonTrouve, bool deb_phr, int accent)
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
        // La forme n'a pas été reconnue
        if (!accent)
            forme = par_position (forme);
        // Si je veux accentuer la forme, c'est ici en else !
        lforme.append (forme);
        return lforme;
    }
    *nonTrouve = false;
    foreach (AnalyseMorpho * am, la)
    {
        QString fq = am->getForm (accent);
        // Si je veux accentuer la forme, c'est dans getForm !
        lforme.append (fq);
    }
    lforme.removeDuplicates ();
    deleteAnalyses (la);
    return lforme;
}

/**************************
 * cherchePieds est une routine récursive qui construit la liste
 * de toutes les combinaisons possibles de dactyles et de spondées
 * en commençant par le caractère d'indice i.
 * Elle est récursive car chaque fois qu'elle a trouvé un D ou un S,
 * elle va chercher toutes les combinaisons possibles à partir
 * du caractère d'indice i+3 ou i+2.
 * Les longues sont notées + et les brèves -.
 * Les mots sont séparés par un espace et on a gardé une trace des élisions avec un `.
 * Une grande partie de la difficulté vient des voyelles communes
 * ou indéterminées, notées *. S'il n'y avait que des + et des -,
 * on n'aurait que D=+-- et S=++. Avec l'* en plus, il faut considérer
 * toutes les possibilités :
 * s=*+, +* ou **
 * d=*--, +*-, +-*, +**, *-*, **- ou ***.
 **************************/
QStringList Lexicum::cherchePieds (int nbr, QString ligne, int i, bool pentam)
{
    QStringList res;
    QString longueurs="+-*";
    int ll=ligne.count();
    if (i>=ll) // trop loin !
    {
        res << "";
        return res;
    }
    if (nbr == 1) // Dernier pied !
    {
//        qDebug () << i << " " << ll << " " << ligne << ligne[i] << ligne[i+1];
        if (pentam)
        {
            // C'est un pentamètre, je ne dois avoir qu'une quantité indifférente
            if (ligne[i]==' ') i+=1;
            // J'étais sur un blanc (espace entre deux mots), j'ai avancé d'une syllabe
            if ((i==ll-1) || (ligne[i+1]==' ')) res << "Z"; // Fin de ligne ou du mot
            else res << "";
            return res;
        }
        else
        {
            // C'est un hexamètre, je cherche encore deux syllabes
            while (!longueurs.contains(ligne[i]) && i<ll) i+=1;
//            qDebug () << i << " " << ll << " " << ligne << ligne[i] << ligne[i+1];
            if (i>ll-2)
            {
                res << "";
                return res;
            }
            if (ligne[i]!='-')
            {
                if (i==ll-2 && longueurs.contains(ligne[i+1]) ) res << "X";
                else if (ligne[i+2]==' ') res << "X";
                else res << "";
            }
            else res << "";
            return res;
        }
    }
    // J'ai traité les cas qui terminent la récursion
    while (!longueurs.contains(ligne[i]) && i<ll) i+=1;
    if (i==ll)
    {   // Encore un cas qui termine
        res << "";
        return res;
    }
    QChar car1=ligne[i];
    int j=i+1;
    while (!longueurs.contains(ligne[j]) && j<ll) j+=1;
    if (j==ll) // Je n'ai qu'une syllabe : fin prématurée de pentamètre ?
    {
        res << "z";
        return res;
    }
    QChar car2=ligne[j];
    QChar car3;
    int k=j+1;
    while (!longueurs.contains(ligne[k]) && k<ll) k+=1;
    if (k==ll) car3=' ';
    else car3=ligne[k];
    if (car1=='-')
    {   // Encore un cas qui termine : aucun pied ne commence par une brève
        res << "";
        return res;
    }
    if (nbr==4 && car1=='+')
        res << Ch::ajoute ("Y",cherchePieds(3,ligne,i+1,true));
    if (nbr==4 && car1=='*')
        res << Ch::ajoute ("y",cherchePieds(3,ligne,i+1,true));
    if (car1=='+' && car2=='+')
        res << Ch::ajoute ("S",cherchePieds(nbr-1,ligne,j+1,pentam));
    if ((car1=='+' && car2=='*')||(car1=='*' && car2=='+')||(car1=='*' && car2=='*'))
        res << Ch::ajoute ("s",cherchePieds(nbr-1,ligne,j+1,pentam));
    if (car1=='+' && car2=='-' && car3=='-')
        res << Ch::ajoute ("D",cherchePieds(nbr-1,ligne,k+1,pentam));
    if (car1=='*' && (car2=='-'||car2=='*') && (car3=='-'||car3=='*'))
        res << Ch::ajoute ("d",cherchePieds(nbr-1,ligne,k+1,pentam));
    if (car1=='+' && ((car2=='*' && (car3=='-'||car3=='*'))||(car2=='-' && car3=='*')))
        res << Ch::ajoute ("d",cherchePieds(nbr-1,ligne,k+1,pentam));
    return res;
}

// écrit le texte scandé.
QString Lexicum::scandeTxt (QString texte, int accent, bool stats)
{
    QString schemaMetric;
    QMap <QString, int> freqMetric;
    bool deb_phr;
    int decalage;
    QStringList vers;
    QStringList formes;
    QStringList aff;
    QStringList lignes = texte.split ("\n");
    foreach (QString ligne, lignes)
    {
//        QStringList separ = ligne.split (Ch::reLimitesMots);
        QStringList separ = Ch::decompose(ligne);
        // J'ai maintenant une liste de formes et une liste de séparateurs
        // la ligne d'origine est la concaténation de separ[i]
        // Les termes pairs sont les séparateurs.
        // Les termes impairs sont les mots.
        // J'ai toujours un dernier séparateur, éventuellement vide.
        // La scansion peut commencer !
        decalage=aff.count();
        if (separ.size() < 3)
        {
            aff.append (ligne + "<br />\n");
            // C'est une ligne vide ou ne contenant pas de lettre :
            // je la laisse comme elle est !
            continue;
        }
        bool nonTr, nonTrSuiv;
        QStringList lforme;
        QStringList lfs = formeq (separ[1], &nonTrSuiv, true, accent);
        schemaMetric = "";
        for (int i=1;i<separ.length();i+=2)
        {
            aff.append (separ[i-1]);
            lforme = lfs;
            nonTr = nonTrSuiv;
            if (i < separ.length ()-2)
            {
                deb_phr = separ[i+1].contains (Ch::rePonct);

                lfs = formeq (separ[i+2], &nonTrSuiv, deb_phr, accent);
                if (!accent)
                {
                    if (AnalyseMorpho::consonnes.contains(lfs[0].at(0).toLower()))
                        for (int j=0;j<lforme.length();++j)
                            Ch::allonge (&lforme[j]);
                    else for (int j=0;j<lforme.length();++j)
                        Ch::elide (&lforme[j]);
                }
            }
            lforme.removeDuplicates ();
            // C'est le bon moment pour extraire le schéma métrique
            if (stats)
            {
                if (nonTr) schemaMetric.append("?"+Ch::versPC(lforme[0])+" ");
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
            if (nonTr) aff.append ("<em>"+lforme[0]+"</em>");
            else aff.append (lforme.join (" "));
            // pour les analyses multiples, je dois insérer des espaces.
        }
        aff.append (separ[separ.length ()-1] + "<br />\n");
        // Je termine la ligne par le dernier séparateur et un saut de ligne.
        if (stats)
        {
            // Je cherche des vers dans la prose
            int ii=0;
            int numMot=1;
            int lsch=schemaMetric.count()-10;
            // Un pentamètre compte au moins 10 syllabes, l'hexamètre 12.
            QString longueurs="+-*";
            QStringList result;
            while (ii < lsch)
            {
                while (!longueurs.contains(schemaMetric[ii]) && ii < lsch)
                    ii+=1;
                // Je suis au début du mot
                result.clear();
                if (ii < lsch && schemaMetric[ii] != '-')
                    result = cherchePieds(6, schemaMetric, ii, false);
                // analyse du résultat
                QString numero;
                numero.setNum (ii);
                QString ajout="";
                foreach (QString item, result)
                {
                    if (item.count()==6)
                    {
                        if (ajout=="") ajout="<span style='color:red' title='"+item;
                        else ajout+="\n"+item;
                        int syllabes=0;
                        for (int a=0; a<6;a++)
                        {
                            if (item[a]=='S'||item[a]=='s'||item[a]=='X')
                                syllabes+=2;
                            if (item[a]=='D'||item[a]=='d')
                                syllabes+=3;
                            if (item[a]=='Y'||item[a]=='y'||item[a]=='Z')
                                syllabes+=1;
                        }
                        int j=ii;
                        int nbMots=1;
                        while (syllabes>0)
                        {
                            if (schemaMetric[j]=='?'||schemaMetric[j]=='@') j+=1;
                            else if (longueurs.contains(schemaMetric[j]))
                            {
                                j+=1;
                                syllabes-=1;
                            }
                            else
                            {
                                nbMots+=2;
                                while (!longueurs.contains(schemaMetric[j])&&(j<schemaMetric.size()))
                                    j+=1;
                            }
                        }
                        QString it=item+" : ";
                        for (j=0;j<nbMots;j++)
                            it+=aff[decalage+numMot+j];
                        if (item.endsWith("Z"))
                            it="<span style='color:red'>"+it+"</span>";
                        vers << it+"<br>\n";
                    }
                }
                if (ajout!="")
                {
// decalage+numMot est le numéro du mot, dans la liste aff, où mon analyse a commencé.
                    aff[decalage+numMot]=ajout+"'>"+aff[decalage+numMot];
                    aff[decalage+numMot+5]=aff[decalage+numMot+5]+"</span>"; // 3 premiers mots en rouge
                }
                while ((schemaMetric[ii] != ' ') && ii < lsch)
                    ii+=1;
                numMot+=2;
                // Je suis sur le blanc qui précède un mot
            }
            // Je remplace les +-* par des signes plus conventionnels
            schemaMetric.replace('-', "∪");
            schemaMetric.replace('+', "‑");
            schemaMetric.replace('*', "∪̲");
/*            schemaMetric.replace('-', "u");
            schemaMetric.replace('+', "-");
            schemaMetric.replace('*', "-\u0306");
*/            aff.append ("&nbsp;<small>"+schemaMetric + "</small>&nbsp;<br>\n");
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
        aff.prepend("<a href='#statistiques'>Statistiques</a> <a href='#analyses'>Analyses</a><br>\n");
        aff.prepend("<a name='texte'></a>");
//        aff.prepend("------------<br/>\n"); // Pour séparer la liste du texte.
        vers.prepend("<hr><a href='#texte'>Texte</a> <a href='#statistiques'>Statistiques</a><br>\n");
        vers.prepend("<a name='analyses'></a>");
        for (int i=0; i<formes.size(); i++)
        {
            QString lg = formes[i];
            while (lg[0]=='0') lg=lg.mid(1);
            vers.prepend (lg + "<br/>\n");
            // En faisant un prepend, j'inverse l'ordre : 
            // le plus fréquent finira premier
        }
        vers.prepend("<hr><a href='#texte'>Texte</a> <a href='#analyses'>Analyses</a><br>\n");
        vers.prepend("<a name='statistiques'></a>");
        vers.append("<a href='#texte'>Texte</a> <a href='#statistiques'>Statistiques</a> <a href='#analyses'>Analyses</a><br>\n");
        aff << vers;
//        aff.prepend("------------<br/>\n"); // Pour séparer la liste du texte.
//        foreach (QString ligne, vers) aff.prepend(ligne);
    }
    return aff.join ("");
}

// Met toute l'information sur la lemmatisation de la forme en XML
QStringList Lexicum::formeXML (QString forme, bool *nonTrouve, bool deb_phr)
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
        lforme.append ("");
        lforme.append (forme+"</orth>");
        return lforme;
    }
    *nonTrouve = false;
    // rassembler les clés
    QStringList cles;
    foreach (QString cle, la.keys ())
    {
       if (!cles.contains (cle)) cles << cle;
    }
    foreach (QString cle, cles)
    {
        QString lin = "<can_form>" + cle + "</trad>";
        QStringList llin = lin.split(":");
        llin[0].append("</can_form><trad lang='" + lingua +"'>");
        lin = llin.join(":");
        lin.replace(">:",">");
        lin.remove("<strong>");
        lin.remove("</strong>");
        QStringList lorth;
        QString lemma;
        foreach (AnalyseMorpho * am, la.values (cle))
        {
            lorth << am->getForm();
            lemma = am->entree()->canonR() + QString::number(am->entree()->numh());
        }
        if (lemma.endsWith("1")) lemma.chop(1);
        lorth.removeDuplicates();
        lin.prepend("<lemma>"+lemma);
        for (int i=0; i<lorth.size(); i++)
        {
            lin.append ( "<orth>" + lorth[i]);
            foreach (AnalyseMorpho * am, la.values (cle))
                if (am->getForm() == lorth[i])
            {
                lin.append ( "<morph>" + am->humain ());
                if (!am->getSuff ().isEmpty())
                    lin.append ( "<encl>" + am->getSuff () + "</encl>");
                lin.append ( "</morph>");
            }
            lin.append ( "</orth>");
        }
        lforme << lin + "</lemma>";
    }
    lforme.removeDuplicates ();
    deleteAnalyses (la);
    return lforme.join("").split("<orth>");
}

// écrit toute l'information sur le texte au format XML.
QString Lexicum::txt2XML (QString texte)
{
    bool deb_phr=true;
    int n_phr=1;
    QStringList aff;
    QStringList lignes = texte.split ("\n");
    foreach (QString ligne, lignes)
    {
//        QStringList separ = ligne.split (Ch::reLimitesMots);
        QStringList separ = Ch::decompose(ligne);
        // J'ai maintenant une liste de formes et une liste de séparateurs
        // la ligne d'origine est la concaténation de separ[i]
        // Les termes pairs sont les séparateurs.
        // Les termes impairs sont les mots.
        // J'ai toujours un dernier séparateur, éventuellement vide.
        // La scansion peut commencer !
        if (separ.size() < 3)
        {
            aff.append (ligne + "<br />\n");
            // C'est une ligne vide ou ne contenant pas de lettre :
            // je la laisse comme elle est !
            continue;
        }
        bool nonTrSuiv;
        QStringList lforme;
        QStringList lfs = formeXML (separ[1], &nonTrSuiv, true);
        for (int i=1;i<separ.length();i+=2)
        {
            if (separ[i-1].trimmed().size() > 0)
                aff << "<ponct>" + separ[i-1].trimmed() + "</ponct>\n";
            if (deb_phr)
            {
                if (n_phr > 1)
                    aff.append("</sentence>\n");
                aff << "<sentence id='" + QString::number(n_phr) + "'>\n";
                n_phr +=1;
            }
            lforme = lfs;
            if (i < separ.length ()-2)
            {
                deb_phr = separ[i+1].contains (Ch::rePonct);

                lfs = formeXML (separ[i+2], &nonTrSuiv, deb_phr);
                // La forme à allonger ou élider est le premier mot que
                // je dois extraire, modifier et remettre.
                if (AnalyseMorpho::consonnes.contains(lfs[1].at(0).toLower()))
                    for (int j=1;j<lforme.length();++j)
                    {
                        QStringList llf = lforme[j].split("<");
                        Ch::allonge (&llf[0]);
                        lforme[j] = llf.join("<");
                    }
                else for (int j=1;j<lforme.length();++j)
                {
                    QStringList llf = lforme[j].split("<");
                    Ch::elide (&llf[0]);
                    lforme[j] = llf.join("<");
                }
            }
//            lforme.removeDuplicates ();
//            if (nonTr) aff.append ("<word>"+separ[i]+"<orth>"+lforme[1]+"</word>");
//            else aff.append ("<word>"+separ[i]+lforme.join ("<orth>")+"</word>\n");
            aff.append ("<word>"+separ[i]+lforme.join ("<orth>")+"</word>\n");
        }
        if (separ[separ.length ()-1].trimmed().size() > 0)
        aff.append ("<ponct>"+separ[separ.length ()-1].trimmed() + "</ponct><br />\n");
        else aff.append ("<br />\n");
        deb_phr = separ[separ.length ()-1].contains (Ch::rePonct);
        // Je termine la ligne par le dernier séparateur et un saut de ligne.
    }
    aff.append("</sentence>\n");
    return aff.join ("");
}

/******************
 *    FRÉQUENCES
 ******************/

bool tri (const QString &s1, const QString &s2)
{
    return s1.toLower() > s2.toLower();
}

QStringList Lexicum::frequences (QString txt, QString lang)
{
    QString exLingua = lingua;
    if (lang != "") lingua = lang;
    // J'essaie d'échanger comptage et lemmatisation Ph.
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
        if (hLem.isEmpty())
            sortie << QString ("%1 (%2, %3, %5)\t%4<br/>\n").arg (numero).arg(nUnic).arg(nAmb).arg (lemme).arg(n);
        else
        {
            // J'ai une liste de lemmes connus : je mets des couleurs !
            QString lem = Ch::ote_diacritiques(lemme.left(lemme.indexOf(",")));
            lem.replace("j","i");
            lem.replace("J","I");
            lem.remove("<strong>");
            lem.remove("</strong>");
            QString format = "%1 (%2, %3, %5)\t<span style=\"color:";
            if (hLem.contains(lem)) format += couleurs[0];
            else format += couleurs[1];
            format += "\">%4</span><br/>\n";
            sortie << format.arg (numero).arg(nUnic).arg(nAmb).arg (lemme).arg(n);
        }
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
    if (lang != "") lingua = exLingua;
    return sortie;
}

void Lexicum::lireHyphen(QString fichierHyphen)
{
    foreach (Entree * e, entrees.values()) e->setHyphen("");
    if (!fichierHyphen.isEmpty())
    {
        QFile Capsa (fichierHyphen);
        if (!Capsa.open (QIODevice::ReadOnly|QIODevice::Text))
        {
            nbErreurs += 1;
            msgErreur.append(tr("Capsam legere nequeo %1:\n%2.\n")
                             .arg(fichierHyphen)
                             .arg(Capsa.errorString()));
/*            QMessageBox::warning(0, tr("Collatinus"),
                                 tr("Capsam legere nequeo %1:\n%2.")
                                 .arg(fichierHyphen)
                                 .arg(Capsa.errorString())); */
            return;
        }

        QTextStream flux (&Capsa);
        flux.setCodec("UTF-8");
        QString linea;
        while (!flux.atEnd ())
        {
            linea = flux.readLine ();
            if (linea.isEmpty () || linea[0] == '!') continue;
            QStringList ecl = linea.split ('|');
            if (ecl.count () != 2)
            {
    #ifdef DEBOG
                qDebug () << "ligne mal formée" << linea;
    #endif
                continue;
            }
            Entree * e = entree (Ch::jviu (ecl[0]));
            if (e!=NULL)
                e->setHyphen (ecl[1]);
    #ifdef DEBOG
            else qDebug () << linea << "erreur lireHyphen";
    #endif
        }
        Capsa.close ();

    }
}

/**
 * Gestion des erreurs
 * */

void Lexicum::clearErrors()
{
    nbErreurs = 0;
    msgErreur = "";
}

int Lexicum::numberOfErrors()
{
    return nbErreurs;
}

QString Lexicum::errorMessage()
{
    return msgErreur;
}

void Lexicum::setOptionComm(int oc)
{
    optionCommune = oc;
}

void Lexicum::setOptionHyphen(bool oh)
{
    cesure = oh;
}
