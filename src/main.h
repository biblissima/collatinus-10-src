/*                 main.h
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
 *  You should have received a copy of the GNU General Public License along
 *  with COLLATINVS; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 *  BOGUES & AGENDA  
 *
 
    - fichiers récents dans Settings.
    - esse + dat. omniprésent
    - conspectus : vérifier le Jeanneau, qui semble confondre in et e
      conspectu(m)
    - implémenter la lemmatisation de la sélection si la touche Maj est
      enfoncée, par exemple, ou la proposer dans un contextuel.
 *  
 */

#ifndef MAIN_H
#define MAIN_H

#include "ui_collatinus.h"
#include <QSettings>
#include <QApplication>
#include <QtGui>
#include <QTextEdit>
#include "libcollatinus.h"
#include "lewis.h"
#include "maj.h" // mise à jour à partir du serveur distant

class fenestra;

class Editeur : public QTextEdit
{
    Q_OBJECT

    private:
         TLexicum * lexicum;
         QStringList listek;
         QStringList listekr; // canons ramistes
         QStringList listekq;
         QString ponctPhr; // = "[\\.\\;\\:\\?\\!]";
         QRegExp sepPhr;
         fenestra * fen;
         bool scand; // armé si l'onglet Quantites est visible
    public:
         Editeur (QWidget *parent, const char *name, TLexicum * l, fenestra *f);
         QString motCourant (QTextCursor C);
         bool debPhr (QTextCursor C);
         QString lemmatiseTxt (bool alpha=0, bool cumVocibus = false);
		 QString lemmatiseTxt_expr (bool alpha = false);
         int nbk (); // nombre de lemmes dans la listek
         QString kAt (int n); // lemme n° n dans la listek
         void purge();
         void videListek ();
         void setScand (bool s);
         bool scande ();
    protected:
         void mousePressEvent(QMouseEvent *event);
         void mouseMoveEvent (QMouseEvent *event);
         void mouseReleaseEvent (QMouseEvent *event);
    signals:
         void copie (QString, QString);
    public slots:
         void changeMajPert (bool m);
         QStringList req ();
};

class fenestra : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

    public:
        fenestra (QString url);
        virtual ~fenestra ();
        void flechis (Tentree * e);
        int editeurCourant ();
        QTextEdit * editeurRes ();
    private:
        void majDic ();
        Editeur * EditLatin;
        void createActions();
        QActionGroup * grCibles;
        QTranslator * translatorF;
        QTranslator * translatorE;
        TLexicum * lexicum;
        void clearModif ();
        void capsamInLatinum (const QString &fileName);
        bool capsaminDiscum (const QString &fileName);
        bool cautio ();
        QString repertoire;
        QString capsaIn; // nom du fichier chargé
        QString capsaEx; // nom fu fichier enregistré
        QString nomen; // nom sans chemin ni extension ni _L ni _Q.
        QString dernierT; // nom du fichier à enregistrer dans les préférences.
        void daFichierCourant (QString fileName);
        HttpWindow * httpWin;
        QString quaerendum;
        bool inuentum;
        bool licetMorpho;
        ListeDic listeD;
        bool idx_lu;
        void deest ();
        QString lang;
        QString ante, post;

    private slots:
        void auxilium ();
        void decollatino ();
        void calepin ();
        void lexica_addere_corrigere ();
        void inuenire ();
        void inuenire_denuo ();
        void legere ();
        void lemmatiseTout (bool alpha=false);
        void alpha ();
        void noua ();
        bool scribere ();
        bool scribereVt ();
        void setCible ();
        void vide_texte ();
        void change_syntaxe ();
        void change_morpho (bool m);
        void affiche_lien (QUrl url);
        void change_glossarium (QString nomDic);
        void ecris_prefs ();
        void lis_prefs (QString url="");
        QString langInterf ();

    public slots:
        void langue_interface ();
        void lemmataRadere ();
        void frequences ();
        void controleIcone (int o);
        void affiche_lemmes_dic (QStringList lk, int no=0);
        void affiche_lemme_saisie (bool litt=false);
        void affiche_lemme_saisie_litt ();
        void affiche_flexion_saisie ();
        void change_page_djvu (int p);
        void clicAnte ();
        void clicPost ();
    protected:
        bool event (QEvent *event);
        void closeEvent(QCloseEvent *event);
};

#endif
