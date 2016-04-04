/*                           main.cpp
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

#include <QCoreApplication>
#include "main.h"
#include <QtGui>
#ifdef Q_OS_MAC
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QToolTip>
#include <QInputDialog>
#include <QString>
#else
#include <QDesktopServices>
#include <QProcessEnvironment>
#endif   

#include "syntaxe.h"

/**
 * Notes Mac OS X specifiques :
 *      Q_OS_MAC est defini par Qt si la platforme cible est Mac Os
 *      Titre exact du Georges : K.E. Georges Lateinisch-Deutschen (1913)
 */

// variables globales
// string uia;
QString qsuia;
int minRaritas = 1; // 1 : traite tous les mots ; 4 : seulement les plus rares
bool morphologia = true; // autorise/interdit l'affichage des morphologies.
bool syntaxis = true;
bool calepino;

// prima renvoie le premier mot de l
QString prima (QString l)
{
    QRegExp re ("^\n?(\\w+)");
    int pos = re.indexIn(l);
    if (pos > -1)
    {
       return re.cap (1);
    }
    return "";
}

/**
 * La classe Editeur reimplemente mousePressEvent,
 * qui n'est pas capturé par défaut dans QTextEdit.
 * une instance de cette classe est insérée
 * manuellement dans la partie haute de la fenêtre.
 */
Editeur::Editeur (QWidget *parent, const char *name, TLexicum * l, fenestra * f) : 
    QTextEdit(tr(""), parent)
{
    setAccessibleName(QString::fromUtf8(name));
    lexicum = l;
    scand = false;
    fen = f;
    ponctPhr = "[\\.\\;\\:\\?\\!]";
    sepPhr = QRegExp (ponctPhr);
}

void Editeur::changeMajPert (bool m)
{
    lexicum->changeMajPert (m);
}

QString Editeur::lemmatiseTxt_expr (bool alpha)
{
    QTextCursor C (document ());
    QTextCursor tc2 (document ());
    Phrase * P;
    QStringList lignes;
    QStringList echecs;
    while (!C.atEnd ())
    {
        tc2 = document ()->find (QRegExp ("[\\.\\;\\:\\?\\!]"), C);
        if (tc2.isNull ()) 
            C.movePosition (QTextCursor::End, QTextCursor::KeepAnchor); 
        else C.setPosition (tc2.position (), QTextCursor::KeepAnchor); 
        P = new Phrase (C.selectedText (), lexicum);
        QStringList * sl = P->analyse_et_lemmes ();
        QString l;
        for (int i=0;i<sl->size();i++) 
        {
            l = sl->at (i);
            if (alpha && (l == P->mot_no (i)->getGraphie ()+" ?"))
            {
                echecs << l+"<br/>";
                continue;
            }
            if (!l.isEmpty ())
            {
                // rétablir les lignes
                QStringList ll = sl->at (i).split ("<br>\n");
                foreach (QString lin, ll)
                {
                    lin = lin.trimmed ();
                    if (lin.isEmpty ()) continue;
                    if (lin == P->mot_no (i)->getGraphie ()+" ?")
                        lin = QString ("<span style=\"color:blue;\">%1</span>").arg (lin);
                    lignes << lin;
                }
            }
        }
        delete sl;
        delete P;
        if (!C.atEnd ()) C.movePosition (QTextCursor::Right);
        // qApp->processEvents ();
    }
    if (alpha)
    {
        lignes.removeDuplicates ();
        qSort (lignes.begin (), lignes.end (), sort_i);
    }
    if (!echecs.empty ())
    {
        lignes.append ("<br/>\n<strong>HAEC NON RECOGNITA</strong><br/>\n");
        lignes.append (echecs);
    }
    moveCursor (QTextCursor::Start);
    return lignes.join ("<br/>\n");
}

void Editeur::mousePressEvent (QMouseEvent *event)
{
    QTextEdit::mousePressEvent (event); 
}

void Editeur::mouseMoveEvent (QMouseEvent *event)
{
    QTextEdit::mouseMoveEvent (event); 
}

void Editeur::mouseReleaseEvent (QMouseEvent *event)
{
    // TODO implémenter la lemmatisation de la selection
    // si la touche Maj est enfoncee, par exemple,
    // ou la proposer dans un contextuel ?
    QPoint P = parentWidget()->mapFromGlobal(event->globalPos());
    QTextCursor C = cursorForPosition (P);
    QString mc = motCourant (C);
    if (mc.isEmpty ()) return;
    bool deb_phr = debPhr (C);
    int ci = fen->editeurCourant (); 
    QStringList lfq; // liste de la forme mc avec toutes ses scansions
    QStringList llm; // liste des lemmatisations
    QStringList lfl; // liste des flexions
    QStringList lnk; // ligne d'hyperliens pour la flexion
    Tentree * e;
    Tentree * ae=NULL; // entrée, ancienne entrée
    QString k, kr;
    int mot_num = -1;
    ListeAnalyses analyses = lexicum->lanalyses (mc, deb_phr);
    int pos = C.position ();
    bool syntaxevue = false;
    listekr.clear ();
    for (std::multimap<QString, AnalyseMorpho*>::iterator it = analyses.begin (); it != analyses.end (); ++it)
    {
        e = it->second->entree ();
        // k = e->canon ();
        kr = e->canonR ();
        if (!e->egale (ae) || ci == 2)
        {
            switch (ci)
            { 
                case 0: // lemmatisation
                    if (calepino)
                        llm.append ("\n<br/><strong>" + e->quantite () + "</strong>" + lexicum->ambrogio (e));
                    else llm << e->definition (lexicum->lang ());
                    if (syntaxis && ! syntaxevue)
                    {
                        syntaxevue = true;
                        int debut = 0;
                        QTextCursor tc2 = document ()->find (QRegExp ("[\\.\\;\\:\\?\\!]"), C, QTextDocument::FindBackward);
                        if (tc2.isNull ()) tc2 = QTextCursor (document ());
                        debut = tc2.position ();
                        tc2.setPosition (C.position (), QTextCursor::KeepAnchor); 
                        QStringList lm = tc2.selectedText ().split (QRegExp ("\\W+"));
                        mot_num = -1;
                        foreach (QString elm, lm)
                            if (!elm.isEmpty ()) mot_num++;
                        //int mot_num = lm.count () - 1; // -2 risque de faire planter
                        //if (mot_num > 0) mot_num--; // pourquoi ? cf ligne prec.
                        // si la phrase n'est pas encore analysée, provoquer l'analyse
                        if (!phrase_at_pos (pos))
                        {
                            tc2.setPosition (debut); 
                            QTextCursor tc3 = document ()->find (sepPhr, tc2);
                            tc2.setPosition (tc3.position (), QTextCursor::KeepAnchor);
                            cree_phrase (tc2.selectedText (), debut, tc3.position (), lexicum);
                        }
                        // appeler l'analyse syntaxique du mot
                        if (mot_num < 0) ++mot_num;
                        QString as = analyse_syntaxique (pos, mot_num);//, k);
                        if (!as.isEmpty ())
                            llm.append (as);
                    }
                    break;
                case 2: // scansion 
                    {
                        lfq.append (it->second->getForm ());
                    }
                    break;
                case 3: // flexion
                    lfl << "<div id=\"" + e->quantite () + "\">" + lexicum->flechis (e) + "</div>\n";
                    lnk.append (e->quantite ());
                    break;
            }
            listekr.append (kr);
            ae = e;
        }
    }

    switch (ci)
    {
        case 0: // lemmatisation
            if (!llm.isEmpty ())
            {
                llm.removeDuplicates ();
                fen->editeurRes ()->append (llm.join ("<br/>\n"));
            }
            else fen->editeurRes ()->append (mc+" ?");
            break;
        case 1: // Dictionnaires
            {
                // si la lemmatisation a échoué, travailler avec la forme elle-même
                if (analyses.size () == 0)
                {
                    listekr << mc;
                }
                fen->affiche_lemmes_dic (listekr);
            }
            break;
        case 2: // scansion
            if (!lfq.isEmpty ())
            {
                lfq.removeDuplicates ();
                fen->editeurRes ()->append (lfq.join ("/"));
            }
            else fen->editeurRes ()->append (mc+" ?");
            break;
        case 3: // flexion
            if (!lfl.empty())
            {
                QString liens;
                foreach (QString l, lnk)
                {
                    liens.append ("<a href=\"#"+l+"\">"+l+"</a>&nbsp;");
                }
                liens.append ("<br/n>\n");
                QString html;
                for (int i=0;i<lnk.length ();++i)
                {
                    html.append ("<div id=\""+lnk[i]+"\">");
                    html.append (liens);
                    html.append (lfl[i]);
                    html.append ("</div>\n");
                }
                fen->editeurRes ()->clear ();
                fen->editeurRes ()->append (html);
                fen->editeurRes ()->moveCursor (QTextCursor::Start,QTextCursor::MoveAnchor);
            }
            else fen->editeurRes ()->append ("id non intellexi");
    }
    lexicum->deleteAnalyses (analyses);
    QTextEdit::mouseReleaseEvent (event);
}

/**
 * renvoie le mot sous le curseur C
 *
 */
QString Editeur::motCourant (QTextCursor C)
{
    C.select (QTextCursor::WordUnderCursor);
    QString ret = C.selectedText ();
    ret.remove (QRegExp ("\\W"));
    return ret;
}

bool Editeur::debPhr (QTextCursor C)
{
    QChar car = 'a';
    while (car.isLetter () && C.position () > 0)
    { 
        C.movePosition (QTextCursor::Left, QTextCursor::KeepAnchor);
        car = C.selectedText ().at (0);
    }
    while (!car.isPunct () && !car.isLetter () && C.position () > 0)
    {
       C.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
       car = C.selectedText ().at (0);
    }
    return ponctPhr.contains (car) || C.position () == 0; 
}

void Editeur::videListek ()
{
    listekr.clear ();
}

void Editeur::setScand (bool s)
{
    scand = s;
}

bool Editeur::scande ()
{
    return scand;
}

QStringList Editeur::req ()
{
   return listekr;
}

void fenestra::majDic ()
{
    comboGlossaria->clear ();
    QDir chDicos (qsuia+"/dicos");
    QStringList lcfg = chDicos.entryList (QStringList () << "*.cfg");
    QStringList ldic;
    foreach (QString fcfg, lcfg)
    {
        Dictionnaire * d = new Dictionnaire (fcfg);
        listeD.ajoute (d);
        ldic << d->nom ();
    }
    comboGlossaria->insertItems (0, ldic); 
}

fenestra::fenestra (QString url)
{
    translatorF = new QTranslator;
    translatorE = new QTranslator;
    translatorF->load(qsuia+"collatinus_fr.qm");
    translatorE->load(qsuia+"collatinus_en.qm");
    setupUi(this);
    // ajout dynamique des dictionnaires
    majDic ();
    lexicum = new TLexicum ();
    // dresser la liste des ressources. Module maj.
    actionAuxilium->setShortcut(QKeySequence::HelpContents);
    action_Noua->setShortcut(QKeySequence::New    );
    action_Onerare->setShortcut(QKeySequence::Open   );
    actionScribere_s_ub->setShortcut(QKeySequence::SaveAs );
    action_Scribere->setShortcut(QKeySequence::Save   );
    actionInuenire->setShortcut(QKeySequence::Find   );
    actionInuenire_etiam->setShortcut(QKeySequence::FindNext);
    actionMaiores_litteras->setShortcut(QKeySequence::ZoomIn );
    actionMinores_litteras->setShortcut(QKeySequence::ZoomOut);
    grCibles = new QActionGroup (menu_Lemmata);
    foreach (QString cle, lexicum->cible ().keys ()) 
    {
        QAction * action = new QAction (grCibles);
        action->setText (lexicum->cible ()[cle]);
        action->setCheckable (true);
        menu_Lemmata->addAction(action);
        connect(action, SIGNAL(triggered ()), this, SLOT(setCible ()));
    }
    menu_Lemmata->addAction (actionCalepino);

#ifdef Q_OS_WIN
    //ajout des preferences dans le menu
    QWidget* stretchWidget = new QWidget;
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif
    EditLatin = new Editeur (this, "EditLatin", lexicum, this);
    EditLatin->setObjectName (QString::fromUtf8("EditLatin"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(13), static_cast<QSizePolicy::Policy>(13));
    setSizePolicy(sizePolicy3);
    setMouseTracking(true);
    /*
       splitter->setStyleSheet ("QSplitter::handle:vertical {"
       "height: 10px;"
       "background-color: red;}");
    */
    splitter->insertWidget(0,EditLatin);
    createActions ();
    lis_expr (qsuia + "expressions.fr");
    lis_prefs (url);
    cree_texte ();
    httpWin = NULL;
}

fenestra::~fenestra ()
{
    // delete lexicum;
}

void fenestra::decollatino ()
{
    QMessageBox::about(this, 
         tr("De Collatino"), trUtf8 (
         "COLLATINVS\nLinguae latinae lemmatizatio \n"
         "Licentia GPL, \u00a9 Yves Ouvrard, annis 2009 - 2014 \n" 
         "Versio " VERSION "\n" 
         "Nonnullas partes operis scripsit Philippe Verkerk\n"
         "Gratias illis habeo :\n" 
         "William Whitaker \u2020\n" 
         "Jose Luis Redrejo,\n" 
         "Georges Khaznadar,\n" 
         "Matthias Bussonier,\n" 
         "Gérard Jeanneau,\n" 
         "Jean-Paul Woitrain,\n" 
         "Perseus Digital Library <http://www.perseus.tufts.edu>"));
}

// ecriture et lecture des preferences

void fenestra::ecris_prefs ()
{
    QSettings settings(qApp->organizationName (), qApp->applicationName ());
    settings.beginGroup ("fichiers");
    settings.setValue ("texte", dernierT);
    settings.setValue ("repertoire", repertoire);
    settings.endGroup();
    settings.beginGroup ("lemmatisation");
    settings.setValue ("syntaxis", syntaxis);
    settings.setValue ("cum_uocibus", actionCum_textus_uocibus->isChecked ());
    settings.setValue ("Maj_pert", actionMaj->isChecked ());
    settings.endGroup();
    settings.beginGroup ("langues");
    settings.setValue ("cible", lexicum->lang ());
    settings.setValue ("calepin", actionCalepino->isChecked ());
    settings.setValue ("interface", langInterf ());
    settings.setValue ("dictionnaire", comboGlossaria->currentIndex ());
    settings.endGroup();
    settings.beginGroup ("police");
    settings.setValue ("zoom", EditLatin->currentFont ().pointSize ());
    settings.endGroup();
}

void fenestra::lis_prefs (QString url)
{
    QSettings settings(qApp->organizationName (), qApp->applicationName ());
    settings.beginGroup ("fichiers");
    capsaIn = url;
    if (capsaIn.isEmpty ())
        capsaIn = settings.value ("texte").toString ();
    if (capsaIn.isEmpty ())
        capsaIn = qsuia+"lucretia.txt";
    capsamInLatinum (capsaIn);
    setWindowTitle(tr("%1[*] - %2").arg(nomen).arg(tr("Collatinus")));
    nomen = QFileInfo (capsaIn).baseName ();
    nomen.replace ("_Q", "");
    nomen.replace ("_L", "");
    repertoire = settings.value ("repertoire").toString ();
    settings.endGroup();

    settings.beginGroup ("lemmatisation");
    syntaxis = settings.value ("syntaxis").toBool ();
    actionSyntaxis->setChecked (syntaxis);
    change_syntaxe ();
    actionCum_textus_uocibus->setChecked (settings.value ("cum_uocibus").toBool ());
    actionMaj->setChecked (settings.value ("Maj_pert").toBool ());
    EditLatin->changeMajPert (actionMaj->isChecked ());
    settings.endGroup();

    settings.beginGroup ("langues");
    QString l = settings.value ("cible").toString ();
    if (l.isEmpty ()) l="fr";
    lexicum->dicLinguam (l);
    foreach (QAction * action, grCibles->actions ())
    {
        if (action->text () == lexicum->cible ()[l])
            action->setChecked (true);
    }
    actionCalepino->setChecked (settings.value ("calepin").toBool ());
    calepin ();
    QString li = settings.value ("interface").toString ();
    if (li == "fr") actionLFrancais->setChecked (true);
    else if (li == "en") actionLEnglish->setChecked (true);
    else actionLLatina->setChecked (true);
    langue_interface ();
    comboGlossaria->setCurrentIndex (settings.value ("dictionnaire").toInt ());
    change_glossarium (comboGlossaria->currentText ());
    settings.endGroup();

    settings.beginGroup ("police");
    int t = settings.value ("zoom").toInt ();
    QFont police = EditLatin->currentFont ();
    police.setPointSize (t);
    EditLatin->setFont (police);
    EditTextus->setFont (police);
    EditFelix->setFont (police);
    EditQuantites->setFont (police);
    if (police.pointSize () < 12)
        police.setPointSize (12);
    EditFlexio->setFont (police);
    settings.endGroup();
}

/**
 * cautio retourne Vrai si les editeurs n'ont pas
 * ete modifies, ou si l'utilisateur desire
 * les sauver.
 *
 */
bool fenestra::cautio ()
{
    if (EditLatin->document()->isModified()
       || EditTextus->document()->isModified() 
       || EditQuantites->document()->isModified())
       {
           int ret = QMessageBox::warning(this, tr("Collatinus"),
                   tr("Opus tuum scriptum non est. "
                      "Id facere uis nunc ?"),
                   QMessageBox::Yes | QMessageBox::Default,
                   QMessageBox::No,
                   QMessageBox::Cancel | QMessageBox::Escape);
           if (ret == QMessageBox::Yes)
               return scribere ();
           else if (ret == QMessageBox::Cancel)
               return false;
    }
    return true;
}

bool fenestra::capsaminDiscum (const QString &fileName)
{
    if (fileName.isEmpty ())
        return false;
    int ci = tabWidget->currentIndex ();
    if (ci > 3) return false;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
        QMessageBox::warning(this, tr("Collatinus"),
                             tr("Capsam nequeo scribere %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString debutLexique = "\n---lexique---\n";
    // selon l'onglet
    if (ci == 0)  // texte
    {
        // tester le suffixe
        if (fileName.contains (".htm"))
        {
            out << EditLatin->toHtml ();
            out << debutLexique << endl;
            out << EditTextus->toHtml ();
        }
        else 
        {
            out << EditLatin->toPlainText();
            out << debutLexique << endl;
            out << EditTextus->toPlainText ();
        }
    }
    else if (ci == 2) // quantités
    {
        if (fileName.contains (".htm"))
        {
            out << EditLatin->toHtml ();
            out << debutLexique << endl;
            out << EditQuantites->toHtml ();
        }
        else 
        {
            out << EditLatin->toPlainText();
            out << debutLexique << endl;
            out << EditQuantites->toPlainText ();
        }
    }
    file.close ();
    QApplication::restoreOverrideCursor();

    daFichierCourant(fileName);
    statusBar()->showMessage(tr("Capsa scripta"), 2000);
    dernierT = fileName;
    return true;
} // capsaminDiscum ()

bool fenestra::scribere ()
{
    if (capsaEx.isEmpty())
    {
        return scribereVt ();
    } 
    else 
    {
        dernierT = capsaEx;
        return capsaminDiscum (capsaEx);
    }
}

bool fenestra::scribereVt ()
{
    QString format, ext;
    QString LQ = "";
    switch (tabWidget->currentIndex ())
    {
        case 0: // lemmatisation
            format = tr ("html (*.html);;txt (*.txt);;quam libet (*)");
            LQ = "_L";
            ext = "html";
            break;
        case 2: // quantités
            format = tr ("textus (*.txt);;html (*.html;;quam libet (*)");
            LQ = "_Q";
            ext = "txt";
            break;
        default:
            return false;
    }
    QString nom = nomen + LQ;
    capsaEx = QFileDialog::getSaveFileName (this, tr ("Collatinus - quem capsam ?"), nom, format, &ext);
    if (capsaEx.isEmpty())
        return false;
    // s'assurer de l'extension
    if (ext.contains ("html"))
        ext = ".html";
    else if (ext.contains ("txt"))
        ext = ".txt";
    else if (ext.contains ("tex"))
        ext = ".tex";
    else 
    {
        QFileInfo fi (capsaEx);
        if (fi.suffix ().isEmpty ()) 
        {
            ext = ".txt";
        }
        ext.prepend (".");
    }
    if (!capsaEx.endsWith (ext))
        capsaEx.append (ext);
    return capsaminDiscum (capsaEx);
} // scribereVt ()


/**
 * Charge le fichier nomme fileName dans
 * L'editeur de texte latin (partie superieure).
 *
 */
void fenestra::capsamInLatinum (const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) 
    {
        QMessageBox::warning(this, tr("Collatinus"),
                             tr("Capsam legere nequeo %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString contenu = in.readAll ();
    QTextEdit *te;
    if (capsaIn.contains ("_Q"))
    {
        te = EditQuantites;
        tabWidget->setCurrentIndex (2);
    }
    else 
    {
        te = EditTextus;
        tabWidget->setCurrentIndex (0);
    }
    int debutLexique = contenu.indexOf ("---lexique---");
    if (debutLexique < 0) 
        EditLatin->setPlainText(contenu);
    else
    {
        if (fileName.contains (".htm"))
        {
            EditLatin->setHtml(contenu.left (debutLexique));
            te->setHtml (contenu.mid (debutLexique));
        }
        else
        {
            EditLatin->setPlainText(contenu.left (debutLexique));
            te->setPlainText (contenu.mid (debutLexique));
        }
        // éliminer le séparateur "---lexique---";
        te->moveCursor (QTextCursor::Start,QTextCursor::MoveAnchor);
        te->moveCursor (QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        te->textCursor().removeSelectedText();
    }
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Capsa onerata"), 2000);
    // réinitialiser la syntaxe
    vide_phrases ();
    dernierT = fileName;
} // capsamInLatinum 

bool fenestra::event (QEvent *event)
{
    if ((event->type () == QEvent::ToolTip) && (EditLatin->underMouse ()))
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QPoint P = EditLatin->parentWidget()->mapFromGlobal(helpEvent->globalPos());
        QTextCursor muspos = EditLatin->cursorForPosition (P);
        QString mot = EditLatin->motCourant (muspos);
        if (P.x() >= 0 && P.x () < EditLatin->width () 
            && P.y () >= 0 && P.y () < EditLatin->height ())
        {
            bool deb_phr = EditLatin->debPhr (muspos);
            if (mot.isEmpty ())
                return QWidget::event (event);
            QString bulla = lexicum->lemmatiseM (mot, morphologia, deb_phr).join ("<br/>");
            QRect rect(P.x()-20,P.y()-10,40,40);
            QToolTip::setFont (EditLatin->font ());
            QToolTip::showText (helpEvent->globalPos(), bulla.trimmed (), EditLatin, rect);
        }
    }
    return QWidget::event (event);
}

void fenestra::flechis (Tentree * e)
{
    EditFlexio->append ("<div id=\"" + e->quantite () + "/>");
    EditFlexio->append (lexicum->flechis (e));
}

void fenestra::affiche_flexion_saisie ()
{
    QStringList lfl; // liste des flexions
    EditFlexio->clear ();
    ListeAnalyses analyses = lexicum->lanalyses (SaisieFlexion->text ());
    if (analyses.size () == 0)
    {
        EditFlexio->append (SaisieFlexion->text () + " ?");
        return;
    }
    Tentree * ae = NULL;
    QStringList lnk;
    for (std::multimap<QString, AnalyseMorpho*>::iterator it = analyses.begin (); it != analyses.end (); ++it)
    {
        Tentree * e = it->second->entree ();
        if (e != ae) 
        {
            lnk.append (e->quantite ());
            lfl << "<div id=\"" + e->quantite () + "\">" + lexicum->flechis (e) + "</div>\n";
        }
        ae = e;
    }
    lexicum->deleteAnalyses (analyses);
    QString liens;
    foreach (QString l, lnk)
    {
        liens.append ("<a href=\"#"+l+"\">"+l+"</a>&nbsp;");
    }
    liens.append ("<br/n>\n");
                QString html;
                for (int i=0;i<lnk.length ();++i)
                {
                    html.append ("<div id=\""+lnk[i]+"\">");
                    html.append (liens);
                    html.append (lfl[i]);
                    html.append ("</div>\n");
                }
    EditFlexio->insertHtml (html);
}

int fenestra::editeurCourant ()
{
    return tabWidget->currentIndex ();
}


QTextEdit * fenestra::editeurRes ()
{
    int ci = tabWidget->currentIndex ();
    switch (ci)
    {
        case 0: return EditTextus;
        case 1: return EditFelix;
        case 2: return EditQuantites;
        case 3: return EditFlexio;
        default: return NULL;
    }
}

void fenestra::clearModif ()
{
    EditLatin->document()->setModified(false);
    EditTextus->document()->setModified(false);
    EditQuantites->document()->setModified(false);
}

void fenestra::daFichierCourant (QString fileName)
{
    capsaEx = fileName;
    clearModif ();
    setWindowTitle(tr("%1[*] - %2").arg(nomen).arg(tr("Collatinus")));
}

/**
 * effacement de tous les editeurs
 * apres appel eventuel au dialogue
 * de precaution.
 */
void fenestra::noua ()
{
     if (cautio ())
     {
         EditLatin->clear ();
         EditTextus->clear ();
         vide_phrases ();
         EditLatin->setFocus ();
         capsaIn.clear ();
         capsaEx.clear ();
     }
}

void fenestra::legere ()
{
    if (cautio ()) 
    {
        capsaIn = QFileDialog::getOpenFileName(this, "Capsam legere", repertoire);
        if (!capsaIn.isEmpty())
        {
            capsamInLatinum (capsaIn);
            nomen = QFileInfo (capsaIn).baseName ();
            nomen.replace ("_Q", "");
            nomen.replace ("_L", "");
            repertoire = QDir (capsaIn).absolutePath ();
            clearModif ();
            dernierT = capsaIn;
        }
    }
}

void fenestra::lemmataRadere ()
{
    switch (tabWidget->currentIndex ())
    { 
        case 0:
        EditTextus->clear ();
        break;
        case 2:
        EditQuantites->clear ();
        break;
        case 3:
        EditFlexio->clear ();
    }
}

void fenestra::closeEvent(QCloseEvent *event)
{
    if (cautio ()) 
    {
        ecris_prefs ();
        event->accept();
        delete lexicum;
    } 
    else 
    {
        event->ignore();
    }
    if (httpWin != NULL) httpWin->close ();
}

void fenestra::lemmatiseTout (bool alpha)
{
    int ci = tabWidget->currentIndex ();
    QString T;
    if (ci == 0)
    {
        if (syntaxis) T = EditLatin->lemmatiseTxt_expr (alpha);
        else T = lexicum->lemmatiseTxt (EditLatin->toPlainText (),
                                        alpha,
                                        actionCum_textus_uocibus->isChecked ());
    }
    if (ci == 0)
    {
        EditTextus->clear ();
        EditTextus->insertHtml (T); 
    }
    if (ci == 2)
    {
        EditQuantites->clear ();
        EditQuantites->insertHtml (lexicum->scandeTxt (EditLatin->toPlainText ()));
    }
}

void fenestra::alpha ()
{
    lemmatiseTout (true);
}

void fenestra::frequences ()
{
    int ci = tabWidget->currentIndex ();
    if (ci == 0)
    {
    EditTextus->clear ();
    QStringList freq = lexicum->frequences (EditLatin->toPlainText ());
    EditTextus->insertHtml (QString ("<strong>%1 lemmes reconnus</strong><br/>\n").arg (freq.count ()));
    EditTextus->insertHtml (freq.join (""));
    }
    else if (ci == 2)
    {
        EditQuantites->clear ();
        EditQuantites->insertHtml (lexicum->scandeTxt (EditLatin->toPlainText (), true));
    }
}

void fenestra::setCible ()
{
    QAction * action = grCibles->checkedAction ();
    foreach (QString cle, lexicum->cible ().keys ())
    {
        if (lexicum->cible ()[cle] == action->text ())
        {
            lexicum->dicLinguam (cle);
            break;
        }
    }
}

void fenestra::calepin ()
{
    calepino = actionCalepino->isChecked ();
}

/*************************************
 *            DICTIONNAIRES          *
 *************************************/

void fenestra::lexica_addere_corrigere ()
{
    httpWin = new HttpWindow ();
    httpWin->show ();
    httpWin->exec ();
    delete httpWin;
    httpWin = NULL;
    majDic ();
}


void fenestra::affiche_lemmes_dic (QStringList lk, int no) 
{
    if (lk.empty () || no < 0 || listeD.courant () == NULL) 
        return;
    EditFelix->clear ();
    EditFelix->setHtml (listeD.courant ()->page (lk, no));
    saisie_felix->setText (lk.at (no));  
    if (listeD.courant ()->estXml ())
    {
        AnteButton->setText (listeD.courant ()->pgPrec ());
        PostButton->setText (listeD.courant ()->pgSuiv ());
    }
    else 
    {
        AnteButton->setText (tr ("Retro"));
        PostButton->setText (tr ("Porro"));
        labelLewis->setText (QString::number (listeD.courant ()->noPageDjvu ()));
    }
	EditFelix->setTextCursor(QTextCursor(EditFelix->document()));
}

void fenestra::affiche_lemme_saisie (bool litt)
{
    if (saisie_felix->text ().isEmpty ())
        return;
    EditLatin->videListek (); 
    QStringList requete;
    if (!litt)
    {
        ListeAnalyses analyses;
        analyses = lexicum->lanalyses (saisie_felix->text (), true);
        for (std::multimap<QString, AnalyseMorpho*>::iterator it = analyses.begin (); it != analyses.end ();++it)
        {
            Tentree *e = it->second->entree ();
            requete << e->canonR ();
        }
        lexicum->deleteAnalyses (analyses);
    }
    if (requete.empty ()) requete << saisie_felix->text ();
    requete.removeDuplicates ();
    EditFelix->setFocus ();
    affiche_lemmes_dic (requete);
}

void fenestra::affiche_lemme_saisie_litt ()
{
    affiche_lemme_saisie (true);
}


void fenestra::change_page_djvu (int p)
{
    EditFelix->clear ();
    EditFelix->setHtml (listeD.courant ()->pageDjvu (p));
    labelLewis->setText (QString::number (p));
	EditFelix->setTextCursor(QTextCursor(EditFelix->document()));
}

void fenestra::affiche_lien (QUrl url)
{
    if (listeD.courant ()->estXml ())
        return;
    // la ligne de liens en tête de page doit être gardée
    QStringList liens =  listeD.courant ()->links ();
    int no = liens.indexOf (url.toString ());
    if (no < 0) no = 0;
    affiche_lemmes_dic (liens, no);
}

void fenestra::clicAnte ()
{
    EditLatin->videListek (); 
    listeD.courant ()->vide_ligneLiens ();
    if (listeD.courant ()->estXml ())
    {
        affiche_lemmes_dic (QStringList () << AnteButton->text ());
    }
    else
    {
        int p = labelLewis->text ().toInt ();
        if (p > 0)
            change_page_djvu (labelLewis->text ().toInt ()-1);
    }
}

void fenestra::clicPost ()
{
    EditLatin->videListek (); 
    listeD.courant ()->vide_ligneLiens ();
    if (listeD.courant ()->estXml ())
    { 
        affiche_lemmes_dic (QStringList () << PostButton->text ());
    }
    else
    {
        int p = labelLewis->text ().toInt ();
        if (p < 8888)   // ATTENTION, déclarer la dernière page dans les cfg !
            change_page_djvu (labelLewis->text ().toInt ()+1);
    }
}

void fenestra::change_glossarium (QString nomDic)
{
    listeD.change_courant (nomDic);
    if (listeD.courant () == NULL)
        return;
    if (listeD.courant ()->estXml ())
    {
        labelLewis->setText ("\u2194");
    }
    else
    {
        listeD.courant ()->vide_index ();
        labelLewis->clear ();
    }
    QStringList req = EditLatin->req ();
    if (!req.empty ())
    {
        affiche_lemmes_dic (req, req.indexOf (saisie_felix->text ()));
    }
    else if (!saisie_felix->text ().isEmpty())
    {
        affiche_lemmes_dic (QStringList () << saisie_felix->text ());
    }
}

/**************************************
 *    RECHERCHE DANS LE TEXTE LATIN   *
 ***************************************/

void fenestra::inuenire ()
{
    // détecter l'éditeur actif
    QTextEdit * editeur;
    /*
    if (EditTextus->hasFocus ()) editeur = EditTextus;
    else editeur = EditLatin;
    */
    if (EditLatin->hasFocus ()) editeur = EditLatin;
    else editeur = editeurRes ();
    bool ok;
    quaerendum  = QInputDialog::getText(this, tr("Quid quaeris ?"),
                                         tr("Quid quaeris ?"), QLineEdit::Normal,
                                         quaerendum, &ok);
    if (ok && !quaerendum.isEmpty())
    {
        if (!editeur->find (quaerendum))
        {
            quaerendum  = QInputDialog::getText(this, tr("Quid quaeris ?"),
                                                 tr("Ad initium reuerti ?"), QLineEdit::Normal,
                                                 quaerendum, &ok);
            if (ok && !quaerendum.isEmpty())
            {
                // Retourner au debut
                editeur->setTextCursor(QTextCursor(editeur->document()));
                // Chercher à nouveau
                editeur->find(quaerendum);
            }
         }
     }
}

void fenestra::inuenire_denuo ()
{
    bool ok;
    QTextEdit * editeur;
    // if (EditTextus->hasFocus ()) editeur = EditTextus;
    // else editeur = EditLatin;
    if (EditLatin->hasFocus ()) editeur = EditLatin;
    else editeur = editeurRes ();
    if (quaerendum.isEmpty ()) inuenire ();
    else
    {
        if (!editeur->find (quaerendum))
        {
            quaerendum  = QInputDialog::getText(this, tr("Quid quaeris ?"),
                                                 tr("Ad initium reuerti ?"), QLineEdit::Normal,
                                                 quaerendum, &ok);
            if (ok && !quaerendum.isEmpty())
            {
                // Retourner au debut
                editeur->moveCursor (QTextCursor::Start);
                // Chercher à nouveau
                editeur->find(quaerendum);
            }
         }
     }
}

void fenestra::auxilium ()
{
    QDesktopServices::openUrl(QUrl("file:" + qApp->applicationDirPath () + "/doc/index.html"));
}

/**************
 * Syntaxe 
 **************/

void fenestra::change_syntaxe ()
{
    syntaxis = actionSyntaxis->isChecked ();
    // désactiver s'il y a lieu l'option "cum textus uocibus"
    actionCum_textus_uocibus->setChecked(false);
    actionCum_textus_uocibus->setEnabled(!syntaxis);
}

void fenestra::change_morpho (bool m)
{
    morphologia = m;
}


void fenestra::deest ()
{
    EditFelix->clear ();
    QString pg ("Deest lexicum "+comboGlossaria->currentText ()
       + ". Id potes depromere : Lemmata/Lexica addere et corrigere");
    EditFelix->setHtml (pg);
}

void fenestra::vide_texte ()
{
    vide_phrases ();
}

void fenestra::controleIcone (int o)
{
    actionOmnia_lemmatizare->setEnabled (o != 4 && o != 1);
    EditLatin->setScand (o == 2);
    capsaEx.clear ();
}

void fenestra::langue_interface ()
{
     QTranslator myappTranslator;
    if (actionLFrancais->isChecked ()) 
    {
        qApp->removeTranslator (translatorE);
        qApp->installTranslator (translatorF);
    }
    else if (actionLEnglish->isChecked ())
    {
        qApp->removeTranslator (translatorF);
        qApp->installTranslator (translatorE);
    }
    else if (actionLLatina->isChecked ())
    {
        qApp->removeTranslator (translatorF);
        qApp->removeTranslator (translatorE);
    }
    retranslateUi (this);
}

QString fenestra::langInterf ()
{
    if (actionLFrancais->isChecked ())
      return "fr";
    if (actionLEnglish->isChecked ())
      return "en";
    return "la";
}

void fenestra::createActions ()
{
    connect(action_Noua, SIGNAL(triggered()), this, SLOT(noua()));
    connect (action_Onerare, SIGNAL (triggered ()), this, SLOT (legere ()));
    connect (action_Scribere, SIGNAL (triggered ()), this, SLOT (scribere ()));
    connect (actionScribere_s_ub, SIGNAL (triggered ()), this, SLOT (scribereVt ()));
    connect(actionMaiores_litteras, SIGNAL(triggered()), EditLatin, SLOT(zoomIn()));
    connect(actionMinores_litteras, SIGNAL(triggered()), EditLatin, SLOT(zoomOut()));
    connect (actionDe_Collatino, SIGNAL (triggered ()), this, SLOT(decollatino ()));
    connect (actionOmnia_lemmatizare, SIGNAL (triggered ()), this, SLOT(lemmatiseTout ()));
    connect (actionAlphabetice, SIGNAL (triggered ()), this, SLOT(alpha ()));
    connect (action_Frequentiae, SIGNAL (triggered ()), this, SLOT (frequences ()));
    connect(EditLatin, SIGNAL(textChanged()), this, SLOT(vide_texte ())); // actionVide_texte, SLOT(trigger()));
    connect(actionL_emmata_radere, SIGNAL(triggered()), this, SLOT(lemmataRadere()));
    connect(actionCalepino, SIGNAL(triggered ()), this, SLOT (calepin ()));
    connect(actionLexicaAddereCorrigere, SIGNAL(triggered ()), this, SLOT (lexica_addere_corrigere ()));
    connect(actionInuenire, SIGNAL(triggered ()), this, SLOT(inuenire ()));
    connect(actionInuenire_etiam, SIGNAL(triggered ()), this, SLOT(inuenire_denuo ()));
    connect(actionAuxilium, SIGNAL(triggered ()), this, SLOT(auxilium ()));
    connect(actionSyntaxis, SIGNAL(triggered ()), this, SLOT (change_syntaxe ()));
    connect(actionMaj, SIGNAL(toggled (bool)), EditLatin, SLOT (changeMajPert (bool)));
    connect(tabWidget, SIGNAL(currentChanged (int)), this, SLOT (controleIcone (int)));
    connect(actionMorphologia_in_bullis, SIGNAL(toggled (bool)), this, SLOT(change_morpho (bool)));
    connect(saisie_felix, SIGNAL(returnPressed ()), this, SLOT (affiche_lemme_saisie ()));
    connect(SaisieFlexion, SIGNAL(returnPressed ()), this, SLOT (affiche_flexion_saisie ()));
    connect(BFlexion, SIGNAL(clicked ()), this, SLOT (affiche_flexion_saisie ()));
    connect(comboGlossaria, SIGNAL(currentIndexChanged(QString)), this, SLOT (change_glossarium (QString))); // nov
    connect(AnteButton, SIGNAL(clicked ()), this, SLOT (clicAnte ()));
    connect(PostButton, SIGNAL(clicked ()), this, SLOT (clicPost ()));
    // hyperliens dans la page dictionnaire
    connect(EditFelix, SIGNAL(anchorClicked (QUrl)), this, SLOT (affiche_lien (QUrl)));
    connect(felixButton, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie ()));
    connect(bDicoLitt, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie_litt ()));
    // langue d'interface
    QActionGroup * grInterface = new QActionGroup (this);
    actionLFrancais->setActionGroup (grInterface);
    actionLDeutsch->setActionGroup (grInterface);
    actionLEnglish->setActionGroup (grInterface);
    actionLEspanol->setActionGroup (grInterface);
    actionLGallego->setActionGroup (grInterface);
    actionLCatalan->setActionGroup (grInterface);
    actionLLatina->setActionGroup (grInterface);
    connect (actionLFrancais, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLDeutsch, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLEnglish, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLEspanol, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLGallego, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLCatalan, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
    connect (actionLLatina, SIGNAL(triggered ()), this, SLOT (langue_interface ()));
}

int main( int argc, char **argv )
{
#if (QT_VERSION < 0x050000)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    QApplication app (argc, argv);
    // langue des boîtes de dialogue
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);
    // répertoire des ressources
    qsuia = app.applicationDirPath () + "/ressources/";
    //let's set a few variable use to get/load settings
    QCoreApplication::setOrganizationName("Collatinus");
    QCoreApplication::setOrganizationDomain("Collatinus.org");
    QCoreApplication::setApplicationName("Collatinus");
    fenestra f (argv[1]);
    f.show ();
    return app.exec();
}
