#ifndef ENTREE_H
#define ENTREE_H

/*
 *                          entree.h
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


#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTextStream>

// #include "radix.h"
#include "lexicum.h"
#include "irreg.h"

class Irreg;

class Entree : public QObject
{
    Q_OBJECT
private:
    QString k; // canon sans quantités
    QString kq; // canon avec quantités, comme dans le dictionnaire
    QString fcq; // forme canonique entièrement mesurée
    QString kjv; // canon ramiste
    int nh; // numéro d'homonymie
    int mdl;
    QString cf;
//    Lexicum *lexique;
    QMap<int,QStringList> radq;
    QList<Irreg*> irregs;
    QMap<QString,QString> trad;
    QString indMorph; // indications morphologiques
    QList<Entree*> adv; // entrée adj. utilisée pour les degrés d'un ou plusieurs adv.
    // caractéristiques morpho
    bool init_neutre ();
    bool neutre;
    int nombre;
    void init_nombre ();
    bool init_deponent ();
    bool deponent;
    bool init_intrans ();
    bool intrans;
    bool avec_locatif;
//    void init_locatif ();
    void init_radicaux (QStringList eclats);
    void init_quantites ();

public:
//    explicit Entree(QObject *parent = 0);
//    Entree (QString linea, QObject *parent=0);
    Entree (QString linea, QObject *parent = 0);
    virtual ~Entree ();
    bool egale (Entree * e);
    QString canon ();
    QString canonR (); // canon ramiste
    int numh (); // accesseur pour nh
    static QString ote2 (QString k, int &h);
    QString getCf ();
    int modele ();
    char pars ();
    QStringList radicalq (int n);
    QString grmodele ();
    QString quantite ();
    QString definition (QString l); // l : langue
    void ajTrad (QString lang, QString tr);
    void setAdv (Entree * e);
    void setIndMorph (QString im);
    QList<Entree*> getAdv ();
    QString traduction (QString lang);
    QString ambrogio ();
    bool est_neutre ();
    int getNombre ();
    bool aUnSingulier ();
    bool aUnPluriel ();
    bool est_deponent ();
    bool est_intransitif ();
    QList<Irreg*> irreguliers ();
    void ajIrr (Irreg * irr);
    QString doc ();
    bool n_a_pas_trad ();
    QStringList static const modeles ;
    QStringList static const mdlGrecs ;


signals:
    
public slots:
    
};

#endif // ENTREE_H
