/*
 *                          traits.h
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


#ifndef TRAITS_H
#define TRAITS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QBitArray>

class Traits : public QObject
{
    Q_OBJECT
private:
    QBitArray bits;
public:
//    explicit Traits(QObject *parent = 0);
    Traits (int k, int g, int n, int d, int p, int t, int m, int v, QObject *parent = 0);
    Traits (QString postag, QObject *parent = 0);
    virtual ~Traits ();
    QBitArray getBits ();
    QString humain ();
    int casus ();
    int genus ();
    int numerus ();
    int gradus ();
    int persona ();
    int tempus ();
    int modus ();
    int uox ();
    bool contient (Traits * t);
    QStringList static const cass ;
    QStringList static const nombres ;
    QStringList static const degres ;
    QStringList static const personnes ;
    QStringList static const modes ;
    QStringList static const voixs ;
    QStringList static const genres ;
    QStringList static const tempss ;
    QStringList static const refP;
    /*
    QString cas ();
    QString genre ();
    QString nombre ();
    QString degre ();
    QString personne ();
    QString temps ();
    QString mode ();
    QString voix ();
    */

signals:
    
public slots:
    
};

#endif // TRAITS_H
