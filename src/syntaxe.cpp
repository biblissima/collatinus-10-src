/*           syntaxe.cpp
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

#include "syntaxe.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>

namespace Morph
{
    const QString cc="n,v,ac,g,d,ab";
    const QString cg="m,f,nt";
    const QString cn="s,p";
    const QString cd="comp,sup";
    const QString cp="1,2,3";
    const QString ct="pr,fut,impf,pf,fa,pqp";
    const QString cm="ind,subj,imper,inf,part,ger,adjv";
    const QString cv="act,pass";

    const QStringList c = cc.split (",");
    const QStringList g = cg.split (",");
    const QStringList n = cn.split (",");
    const QStringList d = cd.split (",");
    const QStringList p = cp.split (",");
    const QStringList t = ct.split (",");
    const QStringList m = cm.split (",");
    const QStringList v = cv.split (",");

    bool commun (QStringList la, QStringList lb)
    {
        foreach (QString a, la)
            foreach (QString b, lb)
            if  (a==b)
                return true;
        return false;
    }
}

/*********************
 *        MOT        *
 *********************/

Mot::Mot (QString d, Lexicum * l, bool expr)
{
    // abruptus sermo|style haché|0|sermo...|abrumpo.m pf part pass.cn.0
    //    latin        francais  noy  mot 0    mot 1.    morph     .en.avec
    motExpr = expr;
    if (expr)
    {
        gr.clear ();
        noyau = false;
        posExpr = -1;
        nAccordAvec = -1;
        accordEn.clear ();
        QStringList eclats = d.split ('.');
        canons = eclats.at (0).split (',');
        // calcul des traits morphos
        int c=0,g=0,n=0,d=0,p=0,t=0,m=0,v=0;
        pos = -1;
        QStringList et = eclats.at (1).split (' ');
        foreach (QString ecl, et)
        {
            pos = Morph::c.indexOf(ecl); if (pos>=0) c = pos+1; break;
            pos = Morph::g.indexOf(ecl); if (pos>=0) g = pos+1; break;
            pos = Morph::n.indexOf(ecl); if (pos>=0) n = pos+1; break;
            pos = Morph::d.indexOf(ecl); if (pos>=0) d = pos+1; break;
            pos = Morph::p.indexOf(ecl); if (pos>=0) p = pos+1; break;
            pos = Morph::t.indexOf(ecl); if (pos>=0) t = pos+1; break;
            pos = Morph::m.indexOf(ecl); if (pos>=0) m = pos+1; break;
            pos = Morph::v.indexOf(ecl); if (pos>=0) v = pos+1; break;
        }
        Entree * e = NULL;
        foreach (QString canon, canons) 
        { 
            if (!canon.isEmpty ()) 
            {
                e = l->entree (canon);
                analyses << new AnalyseMorpho (canon, new Traits (c,g,n,d,p,t,m,v), e);
            }
        }
        accordEn = eclats.at (2);
        if (!accordEn.isEmpty ())
        {
            nAccordAvec = eclats.at (3).toInt ();
        }
    }
    else
    {
        gr = d;
        int pos = Ch::rePoncDer.indexIn (gr);
        if (pos > -1)
        {
            ponctF = Ch::rePoncDer.cap (1);
            gr.remove (Ch::rePoncDer);
        }
        pos = Ch::rePoncPrem.indexIn (gr);
        if (pos > -1)
        {
            ponctF = Ch::rePoncPrem.cap (1);
            gr.remove (Ch::rePoncPrem);
        }
        ListeAnalyses la = l->lanalyses (gr);
        foreach (QString cle, la.keys ())
        {
            entrees << cle;
            foreach (AnalyseMorpho * am, la.values (cle))
            {
                canons << am->entree ()->canon ();
                analyses << am;
            }
        }
        entrees.removeDuplicates ();
        canons.removeDuplicates ();
    }
}

QString Mot::graphie ()
{
    return gr;
}

QStringList Mot::getCanons ()
{
    return canons;
}

QList<AnalyseMorpho*> Mot::getAnalyses ()
{
    return analyses;
}

void Mot::setNoyau (bool n)
{
    noyau = n;
}

bool Mot::estNoyau ()
{
    return noyau;
}

void Mot::setPos (int p)
{
    pos = p;
}

void Mot::setPosExpr (int p)
{
    posExpr = p;
}

int Mot::getPosExpr ()
{
    return posExpr;
}

void Mot::setAvec (int a)
{
    nAccordAvec = a;
}

int Mot::getAvec ()
{
    return nAccordAvec;
}

void Mot::setEn (QString en)
{
    accordEn = en;
}

QString Mot::getEn ()
{
    return accordEn;
}

bool Mot::accord (Mot * avec)
{
    if (nAccordAvec < 0) return true;
    bool bon = true;
    if (accordEn.contains ('c'))
    {
        bool bc = false;
        foreach (AnalyseMorpho * amm, analyses)
            foreach (AnalyseMorpho * ama, avec->getAnalyses ())
            {
                bc = amm->getTraits ()->casus () == ama->getTraits ()->casus ();
                if (bc && accordEn.contains ('n'))
                {
                    bc = bc && (amm->getTraits ()->numerus () == ama->getTraits ()->numerus ());
                }
                if (bc && accordEn.contains ('g'))
                    bc = bc && (amm->getTraits ()->genus () == ama->getTraits ()->genus ());
                bon = bc;
                if (bon) break;
            }
    }
    else if (accordEn.contains ('g'))
    {
        bool bg = false;
        foreach (AnalyseMorpho * amm, analyses)
            foreach (AnalyseMorpho * ama, avec->getAnalyses ())
            {
                bg = amm->getTraits ()->genus () == ama->getTraits ()->genus ();
                if (bg && accordEn.contains ('n'))
                    bg = bg && (amm->getTraits ()->numerus () == ama->getTraits ()->numerus ());
                bon = bg;
                if (bon) break;
            }
    }
    else if (accordEn.contains ('n'))
    {
        bool bn = false;
        foreach (AnalyseMorpho * amm, analyses)
            foreach (AnalyseMorpho * ama, avec->getAnalyses ())
            {
                bn = (amm->getTraits ()->numerus () == ama->getTraits ()->numerus ());
                bon = bn;
                if (bon) break;
            }
    }
    return bon;
}

QString Mot::lemmatisation ()
{
    return QString ("%1 %2<br/>").arg (entrees.join ("<br />\n")).arg (expr);
}

void Mot::setExpr (QString e)
{
    expr = e.prepend ("<br/>");
}

QString Mot::expression ()
{
    return expr;
}

/*********************
 *     EXPRESSION    *
 *********************/

Expression::Expression (QString lin, Lexicum * l)
{
    // aegre (moleste, grauiter) aliquid ferre|mal supporter qqch.|1|aegre,moleste,grauiter...|fero...|.ac..
    QStringList eclats = lin.split ('|');
    latin = eclats.at (0);
    francais = eclats.at (1);
    noNoyau = eclats.at (2).toInt ();
    for (int i=3;i<eclats.count();++i)
    {
        Mot * m = new Mot (eclats.at (i), l, true);
        if (i-3 == noNoyau) m->setNoyau (true);
        mots << m;
    }
}

QStringList Expression::getNoyau ()
{
    return mots.at (noNoyau)->getCanons ();
}

QList<Mot*> Expression::getMots ()
{
    return mots;
}

QString Expression::humain ()
{
    return QString ("\u00B6 %1 : %2").arg (latin).arg (francais);
}

/*********************
 *       PHRASE      *
 *********************/

Phrase::Phrase (QString txt, int d, int f, Lexicum * l)
{
    debut = d;
    fin = f;
    txt = txt.trimmed ();
    QStringList eclats = txt.split (Ch::reEspace);
    for (int i=0;i<eclats.count();++i)
    {
        Mot * nm = new Mot (eclats.at (i), l);
        nm->setPos (i);
        mots << nm;
    }
}

Phrase::~Phrase ()
{
    foreach (Mot * m, mots)
    {
        delete m;
    }
    mots.clear ();
}

bool Phrase::enPhrase (int p)
{
    return (p >= debut && p <= fin);
}

bool Phrase::aLeCanon (int pe, Mot * me)
{
    bool noyau = me->estNoyau ();
    foreach (Mot * m, mots)
    {
        if (me->getCanons ().empty () || Morph::commun (m->getCanons (), me->getCanons ()))
        {
            if (me->getAnalyses ().empty ()) return true;
            AnalyseMorpho * ame = me->getAnalyses ().at (0); // une seule analyse par mot-expression
            QStringList lte = ame->getTraits ()->humain ().split (" ");
            bool bon = false;
            foreach (AnalyseMorpho * amm, m->getAnalyses ())
            {
                bool b = true;
                QString hm = amm->getTraits ()->humain ();
                foreach (QString te, lte)
                {
                    b = b && (hm.contains (te));
                }
                bon = bon || b;
                if (bon)
                {
                    m->setPosExpr (pe);
                    m->setNoyau (noyau);
                    m->setAvec (me->getAvec ());
                    m->setEn (me->getEn ());
                    return true;
                }
            }
        }
    }
    return false;
}

QList<Mot*> Phrase::getMots ()
{
    return mots;
}

QList<Mot*> Phrase::getMotsExpr ()
{
    QList<Mot*> retour;
    foreach (Mot * m, mots)
        if (m->getPosExpr () >= 0)
            retour << m;
    return retour;
}

void Phrase::resetMotsExpr ()
{
    foreach (Mot * m, mots)
    {
        m->setPosExpr (-1);
        m->setAvec (-1);
        m->setEn ("");
        m->setNoyau (false);
    }
}

/*********************
 *      SYNTAXE      *
 *********************/

Syntaxe::Syntaxe (QString f, Lexicum * l)
{
    lexicum = l;
    QFile fichier (f);
    fichier.open (QIODevice::ReadOnly|QIODevice::Text);
    QTextStream flux (&fichier);
    QString lin;
    while (!flux.atEnd ())
    {
        lin = flux.readLine ();
        if (lin.isEmpty () || lin[0] == '!') continue;
        Expression * exp = new Expression (lin, lexicum);
        foreach (QString n, exp->getNoyau ())
        {
            expressions.insert (n, exp);
        }
    }
    fichier.close ();
    phrase = 0;
}

bool Syntaxe::enPhrase (int p)
{
    if (phrase == 0)
        return false;
    return phrase->enPhrase (p);
}

void Syntaxe::creePhrase (QString txt, int d, int f)
{
    delete phrase;
    phrase = NULL;
    echecs.clear ();
    if (txt.isEmpty ()) return;
    phrase = new Phrase (txt, d, f, lexicum);
}

void Syntaxe::deletePhrase ()
{
    delete phrase;
}

// Expressions candidates pour la phrase
QStringList Syntaxe::exprPhr (QString fc)
{
    QStringList retour;
    foreach (Expression * exp, expressions.values (fc))
    {
        // vérifier que tous les canons de e sont dans la phrase
        phrase->resetMotsExpr ();
        bool bon = true;
        for (int i=0;i<exp->getMots().count();++i)
        {
            Mot * me = exp->getMots ().at (i);
            bon = bon && phrase->aLeCanon (i, me);
        }
        // vérifier les accords
        if (bon)
        {
            bool b = true;
            foreach (Mot * m, phrase->getMotsExpr ())        
            {
                if (m == NULL) continue; 
                b = b && m->accord  (phrase->getMotsExpr ().at (m->getAvec ()));
                if (b && m->estNoyau ()) 
                { 
                    m->setExpr (exp->humain ());
                }
            }
            bon = bon && b;
        }
        if (bon) retour << exp->humain ();
    }
    return retour;
}

QStringList Syntaxe::getEchecs ()
{
    return echecs;
}

QStringList Syntaxe::lemmatisation (bool alpha)
{
    QStringList retour;
    if (phrase == NULL) return retour;
    foreach (Mot * m, phrase->getMots ()) 
    {
        if (m->getCanons ().count () == 0) // échec de la lemmatisation
        {
            if (!alpha) // fil du texte : les non-mots sont admis
                retour << QString ("<span style=\"color:blue;\">%1</span><br/>").arg (m->graphie ());
            else if (!m->graphie ().contains (Ch::reNombre)) // orde alphabétique : éliminer les non-mots
                echecs << m->graphie () + "<br/>";
        }
        else foreach (QString c, m->getCanons ())
        {
            exprPhr (c);
            retour << m->lemmatisation ();
        }
    }
    retour.removeDuplicates ();
    return retour;
}
