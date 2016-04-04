/*                   syntaxe.h
 *
 *  This file is part of PRAELECTOR.
 *                                                                            
 *  PRAELECTOR is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *                                                                            
 *  PRAELECTOR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                            
 *  You should have received a copy of the GNU General Public License
 *  along with PRAELECTOR; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef SYNTAXE_H
#define SYNTAXE_H
#include "lexicum.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>

class Mot: public QObject
{
    Q_OBJECT
    private:
        QString gr;
        QStringList canons;
        QStringList entrees;
        QStringList morpho;
        QList<AnalyseMorpho*> analyses; 
        QString ponctD; // ponctuation avant
        QString ponctF; // ponctuation après
        int pos;        // position dans la phrase
        int posExpr;    // position dans l'expression
        bool motExpr;
        QString expr;
        bool noyau;
        QString accordEn; // accord en (cas, genre, nombre).
        int nAccordAvec; // numéro du mot avec lequel doit s'accorder le mot.
    public:
        Mot (QString d, Lexicum* l, bool expr=false);
        QString graphie ();
        QStringList getCanons ();
        QStringList getEntrees ();
        QList<AnalyseMorpho*> getAnalyses ();
        void setPos (int p);
        void setPosExpr (int p);
        int getPosExpr ();
        void setNoyau (bool n);
        bool estNoyau ();
        void setAvec (int a);
        void setEn (QString en);
        int getAvec ();
        QString getEn ();
        bool accord (Mot * avec);
        void setExpr (QString e);
        QString expression ();
        QString lemmatisation ();
};

class Phrase: public QObject
{
    Q_OBJECT
    private:
        QList<Mot*> mots;
        int debut; // numéro du premier caractère
        int fin;   // numéro du dernier caractère
    public:
        Phrase (QString txt, int d, int f, Lexicum * l);
        ~Phrase ();
        QList<Mot*> getMots ();
        bool enPhrase (int p);
        bool aLeCanon (int pe, Mot * me);
        Mot * getMot (int i);
        QList<Mot*> getMotsExpr ();
        void resetMotsExpr ();
};

class Expression: public QObject
{
    Q_OBJECT
    private:
        QString latin;
        QString francais;
        QList<Mot*> mots;
        int noNoyau; // numéro du noyau
    public:
        Expression (QString lin, Lexicum * l);
        QStringList getNoyau ();
        QList<Mot*> getMots ();
        QString humain ();
};

class Syntaxe: public QObject
{
    Q_OBJECT
    private:
        Lexicum * lexicum;
        QMultiHash<QString,Expression*> expressions;
        Phrase * phrase;
        QStringList echecs; // échecs
    public:
        Syntaxe (QString f, Lexicum * l); // f = fichier des données
        Phrase * getPhrase;
        bool enPhrase (int p);
        void creePhrase (QString txt, int d, int f);
        void deletePhrase ();
        QStringList exprPhr (QString fc);
        QStringList getEchecs ();
        QStringList lemmatisation (bool alpha);
};

#endif
