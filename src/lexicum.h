/*
 *                          lexicum.h
 *                          anciennement libcollatinus.h
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

#ifndef LIBCOLLATINUS_H
#define LIBCOLLATINUS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QBitArray>
#include <QMap>
#include <QMultiMap>
#include <QPair>
#include <QHash>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QApplication>

#include <QtWidgets>
#include <QtNetwork>
#include <QClipboard>

#include <QMessageBox>
// pour déboguer
#include <QDebug>
// pour l'affichage console

#include "traits.h"
#include "analysemorpho.h"
#include "entree.h"
#include "radix.h"
#include "irreg.h"
#include "desinence.h"

/*
 * TODO 
 * 1. Remplacer char Entree::pars () par int
 * Entree::pars (), ces int étant des constantes
 * const int Entree::parsNul = 0;
 * const int Entree::parsNom = 1;
 * const int Entree::parsAdj = 2;
 * const int Entree::parsPron = 3;
 * etc.
 * const int Entree::prepAcc = 18;
 * const int Entree::prepAbl = 19;
 * const int Entree::prepAccAbl = 20; 
 *
 * on pourra ensuite convertir facilement se
 * switch...case de flexio (),
 * et utiliser la propriété pars dans le module de
 * syntaxe.
 *
 * BOGUES
 * - omnis a des degrés de comp.
 *
 */

namespace Ch
{
    /*   Fonctions de conversion  */
    QStringList ajoute (QString mot, QStringList liste);
    void        allonge (QString *f);
    int         aRomano (QString R); // Fonction de conversion des chiffres romains en chiffres arabes.
    bool        commun (QStringList la, QStringList lb); // vrai si la et lb ont au moins un élément commun.
    QStringList decompose (QString ligne);
    void        deQuant (QString *c);
    QString     deramise (QString k);
    bool        estNombre (QString s);
    void        elide (QString *mp);
    QString     ini (QString c, int n); // ini  donne la chaine c tronquée de n caractères à droite
    QString     jviu (QString ch);      // jviu transforme la graphie ramiste en graphie ancienne
    void        minuscules (QString &f);   // toutes les lettres en minuscules sauf la première.
    QString     ote_diacritiques (QString k);
    QString     sans_l (QString f);
    bool        sort_i (const QString &s1, const QString &s2);
    QString     versPC (QString k);
    QString     versPedeCerto (QString k);

    // expressions rationnelles
    const QRegExp reAlphas ("(\\w+)");
    const QRegExp reEspace ("\\s+");
    const QRegExp reLettres ("[A-Za-zæÆœŒ]");
    const QRegExp reLimitesMots ("\\b");
    const QRegExp reNombre ("\\d");
    const QRegExp reNonAlphas ("\\W+");
    const QRegExp reNonLettres ("[^A-Za-zæÆœŒ]");
    const QRegExp rePoncDer ("\\W+$");
    const QRegExp rePoncPrem ("^[\\W]+");
    const QRegExp rePonct ("[\\.?!;:]");
}

/* Définition de types */

class AnalyseMorpho;
typedef QMultiMap<QString, AnalyseMorpho*> ListeAnalyses;
// Liste des analyses morphologiques associée à une forme
// La clef est le lemme.

class Radix;
typedef QMultiMap<QString, Radix*> c2r;
typedef QList<Radix*> lr;
// type liste de radicaux

class Irreg;
typedef QMultiMap<QString, Irreg*> c2i;

class Entree;
typedef QHash<QString, Entree*> c2e;

class Desinence;
typedef QMultiMap<QString, Desinence*> c2d;

/* classe lexique */
class Lexicum: public QObject
{
    Q_OBJECT

private:
    QHash<QString,int> hLem;
    QStringList couleurs;
        QMap <QString, QString> metae;
        bool MajPert;
        // liste d'entrées
        QString lingua; // langue courante
        QString linguaTr; // langue, nom vernaculaire complet
        Traits * bitsComp; // morpho de l'adv au comparatif
        Traits * bitsSup;  // au superlatif
        Traits * bitsCAdv; // morpho de l'adv au comparatif
        Traits * bitsSAdv;  // au superlatif
        // liste des entrées
        c2e entrees;
        QList<Entree*> adjAdv; // entrées d'adj utilisés pour les degrés d'un ou plusieurs adv.
        QList<Entree*> entreesCf; // entrées utlisisant la trad. d'une autre
        // liste des désinences
        c2d desinentiae;
        // liste des irréguliers
        c2i irregulares;
        // liste des modèles des radicaux
        QMap<int,QList<int> > modelesRad;
        c2r radices;
        // liste de radicaux
        typedef QPair<QRegExp,QString> Regle;
        void lemmataLege (QString nomen = "lemmata");
        void pronominaLege (QString nomen = "pronomina");
        void linguamLege (QString lang,QString nomen = "lemmata");
        void traductionesLege (QString nomen = "lemmata");
        QStringList cherchePieds (int nbr, QString ligne, int i, bool pentam);
        QList<Regle> regles;
        // QString voyelles;
        QString ponctPhr;
        ListeAnalyses lanalysesE (QString forme); // enclitiques
        ListeAnalyses lanalysesA (QString forme); // assimilations
        ListeAnalyses corAnalyses (QString forme); // cœur du moteur d'analyse
        QString ListeFlechie (Entree* e, QString clef); // retourne la liste des formes fléchies

    public:
        Lexicum (QString qsuia, QObject *parent = 0);
        void verbaCognita(QString repertoire, bool vb=false);
        void ampliatio ();
        virtual ~Lexicum ();
        QMap <QString, QString> cible ();
        void changeMajPert (bool m);
        Entree *renvoi (Entree *e);  // renvoi d'entrée si la déf. commence par cf.
        Entree * entree (QString k);
        void dicLinguam (QString lang);
        QString lang ();
        QString ambrogio (Entree * e);
        void deleteAnalyses (ListeAnalyses la);
        QString radical (QString r);
        // QString assimile (QString f);
        // QString analyses (QString f);
        // QStringList ajoute (QString mot, QStringList liste);
        void lajoute (QString l, ListeAnalyses& la, AnalyseMorpho * am);
        ListeAnalyses lanalyses (QString forme, bool deb_phr=false);
        QStringList lemmatiseM (QString f, bool mm=true, bool deb_phr = false); // analyse morphologique, début de phrase
        QString lemmatiseTxt (QString &txt, bool alpha=0, bool cumVocibus = false, bool cumMorpho = false);
        QString par_position (QString f);
        QStringList synthA (Entree* e, int c, int n, int g, int d);
        QStringList synthPart (Entree* e, int c, int g, int n, int t, int v);
        QStringList synthN (Entree *e, int c, int n);
        QStringList synthP (Entree* e, int c, int n, int g);
        QStringList synthV (Entree* e, int p, int n, int t, int m, int v);
        QString flechis (Entree* e);
        QString flechis (QString clef); // Cherche la clef parmi les entrées et retourne la liste des formes fléchies
        QStringList formeq (QString forme, bool *nonTrouve, bool deb_phr=true, bool accent=false);
        QString scandeTxt (QString texte, bool accent = false, bool stats = false);
        QStringList formeXML (QString forme, bool *nonTrouve, bool deb_phr);
        QString txt2XML (QString texte);
        QStringList frequences (QString txt);
};

#endif
