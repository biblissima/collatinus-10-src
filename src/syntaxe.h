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

/**
 TODO: transformer la recherche d'un canon ds la phrase :
       QSet Phrases::canons, alimentÃ© par la liste morphos.
 TODO: ajouter la possibilité de plusieurs lemmes pour une position 
       de l'expression, avec la même morpho
       ou même plusieurs lemmes pour la même position, avec des morphos
       différentes
 TODO:
     - le mot noyau est répété : alius... alius ;
     - la même expression est répétée dans la phrase.
*/

#ifndef SYNTAXE_H
#define SYNTAXE_H
#include "libcollatinus.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QMultiMap>

/*************************************************/
// classes côté texte
/*************************************************/

class Morphos: public QObject
{
    Q_OBJECT
    private:
    QString canon;
    QString graphie;
    QStringList elements;
    int item;
    public:
    Morphos (QString mo);
    virtual ~Morphos ();
    void setCanon (QString c);
    QString getCanon ();
    QString getGraphie ();
    QString humain ();
    QString cas ();
    QString genre ();
    QString nombre ();
    void setItem (int i);
    int getItem ();
    bool accepte (Morphos * mb);
};

typedef QList<Morphos *> ListeM;
class Mot: public QObject
{
    Q_OBJECT
    private:
        TLexicum * lexicum;
    QString graphie;
	QString lemme;
    ListeM morphos;
    void lemmatise ();
    bool adopte;
    public:
    Mot (QString g, TLexicum * lx, bool debut = false);
    virtual ~Mot ();
    int no; // ordre du mot dans la phrase
    int count_morphos ();
    QString getGraphie ();
    QString getCanon (int i);
	QString get_lemme ();
    Morphos * getMorpho (int i);
    QString humain (int i);
    QString humain ();
	QString lemmatisation ();
    void setAdopte (bool a);
    bool estAdopte ();
    bool casCommun (Mot * m);
    bool nombreCommun (Mot * m);
    bool deb_phr;
};

/*************************************************/
//           CLASSES CÔTÉ BASE
/*************************************************/

class Canon 
{
    private:
        int id;
        QString graphie; 
    public:
        Canon (QString g);       // création par la graphie
        Canon (int i);           // lecture Ã  partir de la base
};

class Expression
{
    private:
        QString nom; 
        QString doc;
        QList<Morphos *> morphos;
        int noyo; // pos dans l'expr du mot sous lequel elle s'affichera
        int posInP; // position du noyau dans la phrase
        int de;
        QString en;
        int avec;
    public:
        Expression (QString n);        // créateur avec int id
        QString humain ();       // notice affichée par Collatinus
        QString getNom ();
        QString getDoc ();
        QString getEn ();
        QString lemme_noyau ();
        int countMorphos ();
        int getPosInP ();
        void setPosInP (int p);
        Morphos * getMorpho (int im);
        bool accordVoulu ();
};    

typedef QMultiMap<QString, Expression*> L_expressions; 
class Liste_expr
{
    private:
        L_expressions expressions;
    public:
        Liste_expr (QString f);
        ~Liste_expr ();
        QList<Expression*> expr_lemme (QString l);
};


// *************
// CLASSE PHRASE
// *************

// phrase du texte préparée pour le 
// traitement fait par Requete
class Phrase: public QObject
{
    Q_OBJECT
    private:
        QString graphie;
        QList<Mot *> mots;
        QList<Expression *> liste_expressions;
        TLexicum * lexicum;
        void cherche_expressions ();
        int de;
        int avec;
        QString en;
        //bool requis_vus ();
    public:
        int debut;
        int fin; // début et fin pour repérage éventuel dans un texte.
        Phrase (QString t, TLexicum * l);
        virtual ~Phrase ();
        bool aiLeCanon (QString c);
        QList<Expression *> expressions ();
	    Mot * mot_no (int n);
        Mot * motExpr (Morphos * m); // mot non adopté satisfaisant la morpho m
        Expression * expression_no (int n); 
        bool accord (int de, QString en, int avec);
        QString analyse ();
	    QStringList * analyse_et_lemmes ();
        void initAdoptes ();
};


// *************
// CLASSE TEXTE
// *************

class Texte
{
    private:
    QList<Phrase *> phrases;
    public:
    Texte ();
    ~Texte ();
    void ajoute_phrase (Phrase * p);
    void vide_phrases ();
    bool phrase_at_pos (int p);
    Phrase * phrase_pos (int p);
};

void lis_expr (QString ch);

void cree_texte ();

void vide_phrases ();

bool phrase_at_pos (int p);

void cree_phrase (QString p, int d, int f, TLexicum * l);

QString analyse_syntaxique (int p, int mot_no); //, QString &canon);


#endif
