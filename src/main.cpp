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

/**
 * La classe Editeur reimplemente mousePressEvent,
 * qui n'est pas capturé par défaut dans QTextEdit.
 * une instance de cette classe est insérée
 * manuellement dans la partie haute de la fenêtre.
 */
Editeur::Editeur (QWidget *parent, const char *name, Lexicum * l, fenestra * f) : 
    QTextEdit(tr(""), parent)
{
    setAccessibleName(QString::fromUtf8(name));
    lexicum = l;
    scand = false;
    fen = f;
    ponctPhr = ".;:?!";
}

void Editeur::changeMajPert (bool m)
{
    lexicum->changeMajPert (m);
}

QString Editeur::lemmatiseTxt_expr (bool alpha)
{
    QTextCursor C (document ());
    QTextCursor tc2 (document ());

    QStringList lignes;
    QStringList echecs;
    int debut=0, fin=0;
    while (!C.atEnd ())
    {
        debut = C.position ();
        tc2 = document ()->find (QRegExp ("[\\.\\;\\:\\?\\!]"), C);
        if (tc2.isNull ()) 
            C.movePosition (QTextCursor::End, QTextCursor::KeepAnchor); 
        else C.setPosition (tc2.position (), QTextCursor::KeepAnchor); 
        fin = C.position ();
        syntaxe->creePhrase (C.selectedText ().simplified (), debut, fin);
        lignes << syntaxe -> lemmatisation (alpha);
        echecs << syntaxe -> getEchecs ();
        if (!C.atEnd ()) C.movePosition (QTextCursor::Right);
    }
    if (alpha)
    {
        lignes.removeDuplicates ();
        qSort (lignes.begin (), lignes.end (), Ch::sort_i);
    }
    if (!echecs.empty ())
    {
        lignes << "-----<br/>" << "<strong>HAEC NON RECOGNITA:</strong><br/>";
        echecs.removeDuplicates ();
        lignes << echecs;
    }
    moveCursor (QTextCursor::Start);
    return lignes.join ("\n");
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
    Entree * e;
    Entree * ae=NULL; // entrée, ancienne entrée
    QString k, kr;
    ListeAnalyses analyses = lexicum->lanalyses (mc, deb_phr);
    int pos = C.position ();
    listekr.clear ();
    foreach (AnalyseMorpho * am, analyses)
    {
        e = am->entree ();
        kr = e->canonR ();
        if (!e->egale (ae) || ci == 2)
        {
            switch (ci)
            { 
                case 0: // lemmatisation
                    if (calepino)
                        llm.append ("\n<br/>" + e->quantite () + lexicum->ambrogio (e));
                    else llm << e->definition (lexicum->lang ());
                    if (syntaxis)
                    {
                        if (!syntaxe->enPhrase (pos))
                        {
                            QTextCursor tc = document ()->find (Ch::rePonct, C, QTextDocument::FindBackward);
                            int debut = tc.position ();
                            if (tc.isNull ()) tc = QTextCursor (document ());
                            QTextCursor tc2 = document ()->find (Ch::rePonct, C);
                            int fin = tc2.position ();
                            tc.setPosition (fin, QTextCursor::KeepAnchor); 
                            QString txt = tc.selectedText ();
                            syntaxe->creePhrase (txt, debut, fin);
                        }
                        llm << syntaxe->exprPhr (e->canon ());
                    }
                    /*
                    if (syntaxis
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
                    */
                case 2: // scansion 
                    {
                        lfq.append (am->getForm ());
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
    // 13 novembre 2015
    // Cette routine bloque le programme quand j'ai une liste de mots (un par ligne)
    QTextCursor C1 = C;
    C1.movePosition(QTextCursor::StartOfLine);
    int debLigne = C1.position(); // J'introduis la position du début de la ligne
    QChar car = 'a';
    while (car.isLetter () && C.position () > debLigne)
    { 
        if (!C.movePosition (QTextCursor::Left, QTextCursor::KeepAnchor)) continue;
        car = C.selectedText ().at (0);
    }
    if (C.position () == debLigne) return true;
    // En poésie, on trouve souvent une majuscule en début de vers
    while (!car.isPunct () && !car.isLetter () && C.position () > debLigne)
    {
       if (!C.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) continue;
       car = C.selectedText ().at (0);
    }
    if (!car.isLetter () && C.position () == debLigne) return true;
    // Au cas où le vers commencerait par des " (ou autre)
    return ponctPhr.contains (car);
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

void Editeur::setSyntaxe (Syntaxe * s)
{
    syntaxe = s;
}

void fenestra::majDic ()
{
    comboGlossaria->clear ();
    QDir chDicos (qsuia+"/dicos");
    QStringList lcfg = chDicos.entryList (QStringList () << "*.cfg");
//    QStringList ldic;
    ldic.clear();
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
    lexicum = new Lexicum (qsuia);
    syntaxe = new Syntaxe (qsuia+"expressions.fr", lexicum);
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
    EditLatin->setSyntaxe (syntaxe);
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
    lis_prefs (url);
    httpWin = NULL;
    fen_Dic = NULL;
    fen_Opt = NULL;
    extraDicVisible = false;
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
    settings.setValue ("repHyphen", repHyphen);
    settings.setValue ("repVerba", repVerba);
    settings.endGroup();
    settings.beginGroup ("lemmatisation");
    settings.setValue ("syntaxis", syntaxis);
    settings.setValue ("cum_uocibus", actionCum_textus_uocibus->isChecked ());
    settings.setValue ("cum_morphoB", actionMorphologia_in_bullis->isChecked ());
    settings.setValue ("cum_morphoL", actionMorphologia_in_lemmatibus->isChecked ());
    settings.setValue ("Maj_pert", actionMaj->isChecked ());
    settings.setValue ("verif_morpho", verifMorpho);
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
    repHyphen = settings.value ("repHyphen").toString ();
    if (repHyphen.isEmpty()) repHyphen = qsuia;
    repVerba = settings.value ("repVerba").toString ();
    if (repVerba.isEmpty()) repVerba = "~";
    settings.endGroup();

    settings.beginGroup ("lemmatisation");
    syntaxis = settings.value ("syntaxis").toBool ();
    actionSyntaxis->setChecked (syntaxis);
    change_syntaxe ();
    actionMorphologia_in_bullis->setChecked (settings.value ("cum_morphoB").toBool ());
    actionMorphologia_in_lemmatibus->setChecked (settings.value ("cum_morphoL").toBool ());
    verifMorpho = settings.value ("verif_morpho").toBool ();

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
    if (fileName.endsWith(".xml"))
    {
        out << lexicum->txt2XML (EditLatin->toPlainText ());
        // sortie au format xml avec toute l'information
    }
    // selon l'onglet
    else if (ci == 0)  // texte
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
            format = tr ("html (*.html);;xml (*.xml);;txt (*.txt);;quam libet (*)");
            LQ = "_L";
            ext = "html";
            break;
        case 2: // quantités
            format = tr ("textus (*.txt);;xml (*.xml);;html (*.html;;quam libet (*)");
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
    else if (ext.contains ("xml"))
        ext = ".xml";
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

void fenestra::oteDiac()
{
    QString contenu = EditLatin->toPlainText ();
    contenu = contenu.normalized(QString::NormalizationForm_D,QChar::currentUnicodeVersion());
    contenu.remove("\u0301");
    contenu.remove("\u0306");
    contenu.remove("\u0304");
    EditLatin->setPlainText(contenu);
}

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

void fenestra::flechis (Entree * e)
{
    EditFlexio->append ("<div id=\"" + e->quantite () + "/>");
    EditFlexio->append (lexicum->flechis (e));
}

void fenestra::affiche_flexion_saisie ()
{
    EditFlexio->clear ();
    ListeAnalyses analyses = lexicum->lanalyses (SaisieFlexion->text ());
    if (analyses.size () == 0)
    {
        EditFlexio->append (SaisieFlexion->text () + " ?");
        return;
    }
    QStringList lfl; // liste des flexions
    Entree * ae = NULL;
    QStringList lnk;
    foreach (AnalyseMorpho * am, analyses)
    {
        Entree * e = am->entree ();
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
         EditLatin->setFocus ();
         capsaIn.clear ();
         capsaEx.clear ();
     }
}

void fenestra::lireFichierHyphen()
{
    QString ficIn = QFileDialog::getOpenFileName(this, "Capsam legere", repHyphen+"/hyphen.la");
    if (!ficIn.isEmpty()) repHyphen = QFileInfo (ficIn).absolutePath ();
    lexicum->clearErrors();
    lexicum->lireHyphen(ficIn);
    if (lexicum->numberOfErrors()>0)
        QMessageBox::warning(0, tr("Collatinus"),lexicum->errorMessage());
}

void fenestra::tabulaFormae()
{
    capsaIn = QFileDialog::getOpenFileName(this, "Capsam legere", repertoire);
    if (!capsaIn.isEmpty())
    {
        nomen = QFileInfo (capsaIn).baseName ();
        repertoire = QFileInfo (capsaIn).absolutePath ();
        QFile file(capsaIn);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("Collatinus"),
                                 tr("Capsam legere nequeo %1:\n%2.")
                                 .arg(capsaIn)
                                 .arg(file.errorString()));
            return;
        }

        QTextStream in(&file);
        QFile fout(QFileInfo (capsaIn).absolutePath()+"/tabula.csv");
        QFile ferr(QFileInfo (capsaIn).absolutePath()+"/erreurs.txt");
        if (!fout.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("Collatinus"),
                                 tr("Capsam nequeo scribere %1:\n%2.")
                                 .arg("tabula.csv")
                                 .arg(fout.errorString()));
            return;
        }
        if (!ferr.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("Collatinus"),
                                 tr("Capsam nequeo scribere %1:\n%2.")
                                 .arg("erreurs.txt")
                                 .arg(ferr.errorString()));
            return;
        }
        QTextStream out(&fout);
        QTextStream err(&ferr);
        while (!in.atEnd())
        {
            QString ligne = in.readLine();
            if (!ligne.isEmpty())
            {
                QString liste = lexicum->flechis(ligne);
                if (liste.isEmpty()) err << ligne << "\n";
                else out << liste;
            }
        }
        file.close();
        fout.close();
        ferr.close();
    }
}

void fenestra::verbaCognita(bool vb)
{
    QString fichier;
    if (vb) fichier = QFileDialog::getOpenFileName(this, "Verba cognita", repVerba);
    if (!fichier.isEmpty()) repVerba = QFileInfo (fichier).absolutePath ();
    lexicum->verbaCognita(fichier,vb);
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
            repertoire = QFileInfo (capsaIn).absolutePath ();
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
    if (fen_Dic != NULL) fen_Dic->close ();
    if (fen_Opt != NULL) fen_Opt->close ();
}

bool fenestra::verif_morpho ()
{
    if (verifMorpho) return true;
    QMessageBox reponse(this);
    reponse.setText(tr("Haec optio in minoribus computatris lentissima est. Pergere uis?"));
    reponse.addButton(tr("Nolo."), QMessageBox::RejectRole);
    reponse.addButton(tr("Volo"), QMessageBox::AcceptRole);
    reponse.addButton(tr("Volo, Noli iam hoc roga."), QMessageBox::DestructiveRole);
    reponse.setWindowTitle(tr("Cautio"));
    reponse.setIcon(QMessageBox::Question);
    reponse.setWindowModality(Qt::WindowModal);
    switch (reponse.exec()) 
    {
        case QDialog::Accepted:
                return true;
        case QDialog::Rejected:
                return false;
        case QMessageBox::DestructiveRole:
            {
                verifMorpho = true;
                return true;
            }
        default:
                return false;
    }
}

void fenestra::lemmatiseTout (bool alpha)
{
    int ci = tabWidget->currentIndex ();
    QString T;
    if (ci == 0)
    {
        if (syntaxis) T = EditLatin->lemmatiseTxt_expr (alpha);
        else
        {
            QString texte = EditLatin->toPlainText ();
            T = lexicum->lemmatiseTxt (texte,
                                        alpha,
                                        actionCum_textus_uocibus->isChecked (),
                                        actionMorphologia_in_lemmatibus->isChecked () && verif_morpho ());
            if (texte.contains("<span")) EditLatin->setHtml(texte);
            // Le texte a été modifié, donc colorisé.
        }
    }
    if (ci == 0)
    {
        EditTextus->clear ();
        EditTextus->insertHtml (T); 
    }
    if (ci == 2)
    {
        EditQuantites->clear ();
        int accent=0;
        if (alpha) accent = lire_options();
        EditQuantites->insertHtml (lexicum->scandeTxt (EditLatin->toPlainText (), accent, false));
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
        EditQuantites->insertHtml (lexicum->scandeTxt (EditLatin->toPlainText (), false, true));
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

void fenestra::init_fen_Dic()
{
    // Initialisation de la fenêtre supplémentaire, fen_Dic, avec tous les boutons nécessaires.
    // Je me suis inspiré du code généré à partir de Collatinus.ui : ui_Collatinus.h

    fen_Dic->resize(800, 600);
    fen_Dic->setObjectName(QStringLiteral("fen_Dic"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHeightForWidth(fen_Dic->sizePolicy().hasHeightForWidth());
    fen_Dic->setSizePolicy(sizePolicy1);

    vboxLayout22 = new QVBoxLayout(fen_Dic);
    vboxLayout22->setObjectName(QStringLiteral("vboxLayout22"));
    horizontalLayout22 = new QHBoxLayout();
    horizontalLayout22->setObjectName(QStringLiteral("horizontalLayout22"));
    // Un layout vertical avec un layout horizontal en haut et le QTextBrowser en bas

    entree_felix2 = new QLabel(fen_Dic);
    entree_felix2->setObjectName(QStringLiteral("entree_felix2"));
    QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(entree_felix->sizePolicy().hasHeightForWidth());
    entree_felix2->setSizePolicy(sizePolicy3);
    entree_felix2->setTextFormat(Qt::AutoText);
    entree_felix2->setText("?");

    horizontalLayout22->addWidget(entree_felix2);

    saisie_felix2 = new QLineEdit(fen_Dic);
    saisie_felix2->setObjectName(QStringLiteral("saisie_felix"));
    saisie_felix2->setMaximumSize(QSize(859, 16777215));

    horizontalLayout22->addWidget(saisie_felix2);

    felixButton2 = new QPushButton(fen_Dic);
    felixButton2->setObjectName(QStringLiteral("felixButton2"));
    QIcon icon215;
    icon215.addFile(QStringLiteral(":/images/svg/dicolem.svg"), QSize(), QIcon::Normal, QIcon::Off);
    felixButton2->setIcon(icon215);

    horizontalLayout22->addWidget(felixButton2);

    bDicoLitt2 = new QPushButton(fen_Dic);
    bDicoLitt2->setObjectName(QStringLiteral("bDicoLitt2"));
    QIcon icon216;
    icon216.addFile(QStringLiteral(":/images/svg/dicolitt.svg"), QSize(), QIcon::Normal, QIcon::Off);
    bDicoLitt2->setIcon(icon216);

    horizontalLayout22->addWidget(bDicoLitt2);

    comboGlossaria2 = new QComboBox(fen_Dic);
    comboGlossaria2->setObjectName(QStringLiteral("comboGlossaria"));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(comboGlossaria2->sizePolicy().hasHeightForWidth());
    comboGlossaria2->setSizePolicy(sizePolicy4);

    horizontalLayout22->addWidget(comboGlossaria2);

    AnteButton2 = new QPushButton(fen_Dic);
    AnteButton2->setObjectName(QStringLiteral("AnteButton"));
    AnteButton2->setEnabled(true);

    horizontalLayout22->addWidget(AnteButton2);

    labelLewis2 = new QLabel(fen_Dic);
    labelLewis2->setObjectName(QStringLiteral("labelLewis"));
    QFont font3;
    font3.setPointSize(14);
    labelLewis2->setFont(font3);

    horizontalLayout22->addWidget(labelLewis2);

    PostButton2 = new QPushButton(fen_Dic);
    PostButton2->setObjectName(QStringLiteral("PostButton"));

    horizontalLayout22->addWidget(PostButton2);

    horizontalSpacer = new QSpacerItem(36, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    vboxLayout22->addLayout(horizontalLayout22);

    fen_dic = new QTextBrowser (fen_Dic);
    fen_dic->setObjectName(QStringLiteral("fen_dic"));
    QFont font1;
    font1.setBold(false);
    font1.setWeight(50);
    fen_dic->setFont(font1);
    fen_dic->setOpenExternalLinks(true);

    vboxLayout22->addWidget (fen_dic);

    // Ma fenêtre est prête.

    comboGlossaria2->insertItems (0, ldic);
    listeD.change_courant2 (comboGlossaria->currentText ());
    comboGlossaria2->setCurrentIndex(comboGlossaria->currentIndex());

    //  Actions liées au dico
        connect(saisie_felix2, SIGNAL(returnPressed ()), this, SLOT (affiche_lemme_saisie2 ()));
        connect(comboGlossaria2, SIGNAL(currentIndexChanged(QString)), this, SLOT (change_glossarium2 (QString))); // nov
        connect(AnteButton2, SIGNAL(clicked ()), this, SLOT (clicAnte2 ()));
        connect(PostButton2, SIGNAL(clicked ()), this, SLOT (clicPost2 ()));
        // hyperliens dans la page dictionnaire
        connect(fen_dic, SIGNAL(anchorClicked (QUrl)), this, SLOT (affiche_lien2 (QUrl)));
        connect(felixButton2, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie2 ()));
        connect(bDicoLitt2, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie_litt2 ()));
        // fin des actions liées au dico

}

void fenestra::affiche_lemmes_dic2 (QStringList lk, int no)
{
    if (lk.empty () || no < 0 || listeD.courant2 () == NULL)
        return;
    fen_dic->clear ();
    fen_dic->setHtml (listeD.courant2 ()->page (lk, no));
    saisie_felix2->setText (lk.at (no));
    if (listeD.courant2 ()->estXml ())
    {
        AnteButton2->setText (listeD.courant2 ()->pgPrec ());
        PostButton2->setText (listeD.courant2 ()->pgSuiv ());
    }
    else
    {
        AnteButton2->setText (tr ("Retro"));
        PostButton2->setText (tr ("Porro"));
        labelLewis2->setText (QString::number (listeD.courant2 ()->noPageDjvu ()));
    }
    fen_dic->setTextCursor(QTextCursor(fen_dic->document()));
}

void fenestra::affiche_lemme_saisie2 (bool litt)
{
    if (saisie_felix2->text ().isEmpty ())
        return;
    EditLatin->videListek ();
    QStringList requete;
    if (!litt)
    {
        ListeAnalyses analyses;
        analyses = lexicum->lanalyses (saisie_felix2->text (), true);
        foreach (AnalyseMorpho * am, analyses)
        {
            Entree *e = am->entree ();
            requete << e->canonR ();
        }
        lexicum->deleteAnalyses (analyses);
    }
    if (requete.empty ()) requete << saisie_felix2->text ();
    requete.removeDuplicates ();
    fen_dic->setFocus ();
    affiche_lemmes_dic2 (requete);
    saisie_felix2->setFocus();
    saisie_felix2->selectAll();
}

void fenestra::affiche_lemme_saisie_litt2 ()
{
    affiche_lemme_saisie2 (true);
}


void fenestra::change_page_djvu2 (int p)
{
    fen_dic->clear ();
    fen_dic->setHtml (listeD.courant2 ()->pageDjvu (p));
    labelLewis2->setText (QString::number (p));
    fen_dic->setTextCursor(QTextCursor(fen_dic->document()));
}

void fenestra::affiche_lien2 (QUrl url)
{
    if (listeD.courant2 ()->estXml ())
        return;
    // la ligne de liens en tête de page doit être gardée
    QStringList liens =  listeD.courant2 ()->links ();
    int no = liens.indexOf (url.toString ());
    if (no < 0) no = 0;
    affiche_lemmes_dic2 (liens, no);
}

void fenestra::clicAnte2 ()
{
    EditLatin->videListek ();
    listeD.courant2 ()->vide_ligneLiens ();
    if (listeD.courant2 ()->estXml ())
    {
        affiche_lemmes_dic2 (QStringList () << AnteButton2->text ());
    }
    else
    {
        int p = labelLewis2->text ().toInt ();
        if (p > 0)
            change_page_djvu2 (labelLewis2->text ().toInt ()-1);
    }
}

void fenestra::clicPost2 ()
{
    EditLatin->videListek ();
    listeD.courant2 ()->vide_ligneLiens ();
    if (listeD.courant2 ()->estXml ())
    {
        affiche_lemmes_dic2 (QStringList () << PostButton2->text ());
    }
    else
    {
        int p = labelLewis2->text ().toInt ();
        if (p < 8888)   // ATTENTION, déclarer la dernière page dans les cfg !
            change_page_djvu2 (labelLewis2->text ().toInt ()+1);
    }
}

void fenestra::change_glossarium2 (QString nomDic)
{
    listeD.change_courant2 (nomDic);
    if (listeD.courant2 () == NULL)
        return;
    fen_Dic->setWindowTitle("Collatinus : " + listeD.courant2 ()->nom());
    if (listeD.courant2 ()->estXml ())
    {
        labelLewis2->setText ("\u2194");
    }
    else
    {
        listeD.courant2 ()->vide_index ();
        labelLewis2->clear ();
    }
    QStringList req = EditLatin->req ();
    if (!req.empty ())
    {
        affiche_lemmes_dic2 (req, req.indexOf (saisie_felix2->text ()));
    }
    else if (!saisie_felix2->text ().isEmpty())
    {
        affiche_lemmes_dic2 (QStringList () << saisie_felix2->text ());
    }
}

void fenestra::lancerServeur(bool run)
{
    if (run)
    {
        QMessageBox::about(this,
             tr("Serveur de Collatinus"), startServer());

    }
    else
    {
        QMessageBox::about(this,
             tr("Serveur de Collatinus"), stopServer());

    }
}

void fenestra::connexion ()
{
    soquette = serveur->nextPendingConnection ();
    connect (soquette, SIGNAL (readyRead ()), this, SLOT (exec ()));
}

void fenestra::exec ()
{
    QByteArray octets = soquette->readAll ();
    QString requete = QString (octets);
    QString rep = "";
    if (requete[0] == '-')
    {
        char a = requete[1].toLatin1();
        QString options = requete.mid(0,requete.indexOf(" "));
        QString lang = "fr";
        int optAcc = 0;
        requete = requete.mid(requete.indexOf(" ")+1);
        switch (a)
        {
        case 's':
            if ((options.size() > 2) && (options[2].isDigit()))
                optAcc = options[2].digitValue() & 7;
            rep = lexicum->scandeTxt(requete,0,optAcc==1);
            break;
        case 'a':
            optAcc = 7; // Par défaut.
            if ((options.size() > 2) && (options[2].isDigit()))
                optAcc = options[2].digitValue() & 7;
            rep = lexicum->scandeTxt(requete,optAcc,false);
            break;
        case 'l':
            if ((options.size() > 2) && (options[2].isDigit()))
            {
                optAcc = options[2].digitValue();
                options = options.mid(3);
            }
            if ((options.size() > 0) && (options[0].isDigit()))
            {
                optAcc = 10*optAcc+options[0].digitValue();
                options = options.mid(1);
            }
            if ((options.size() == 2) && lexicum->cible().contains(options))
                lang = options;
            if (optAcc > 15) rep = lexicum->frequences(requete,lang).join("");
            else
            rep = lexicum->lemmatiseTxt(requete,optAcc&4,optAcc&2,optAcc&1);
            break;
        case 'x':
            rep = lexicum->txt2XML(requete);
            break;
        case 'c':
            if (options.size() > 2)
                lexicum->changeMajPert(options[2] == '1');
            else lexicum->changeMajPert(false);
            break;
        case 'C':
            lexicum->changeMajPert(true);
            break;
        case 't':
            if (options.size() > 3)
                lexicum->dicLinguam(options.mid(2,2));
            break;
        case '?':
            rep = "Les commandes possibles sont : \n";
            rep += "\t-s : Scansion du texte (-s1 : avec recherche des mètres).\n";
            rep += "\t-l : Lemmatisation du texte (avec options -l0..-l8).\n";
            rep += "\t-a : Accentuation du texte (avec options -a1..-a3 et -a5..-a7).\n";
            rep += "\t-x : Mise en XML du texte.\n";
            break;
        default:
            break;
        }
    }
    else rep= lexicum->scandeTxt(requete);
    rep.remove("<br />");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(rep);
    QByteArray ba = rep.toUtf8();
    soquette->write(ba);
}

QString fenestra::startServer()
{
    serveur = new QTcpServer (this);
    connect (serveur, SIGNAL(newConnection()), this, SLOT (connexion ()));
    if (!serveur->listen (QHostAddress::LocalHost, 5555))
    {
        return "Ne peux écouter.";
    }
    return "Le serveur est lancé.";
}

QString fenestra::stopServer()
{
    serveur->close();
    delete serveur;
    return "Le serveur est éteint.";
}

void fenestra::extra_dico(bool visible)
{
    if (fen_Dic == NULL)
    {
        // Elle n'existe pas, je dois la créer
        fen_Dic = new QWidget ();
        init_fen_Dic ();
    }
    extraDicVisible = visible;
    if (visible)
    {
        fen_Dic->show();
        fen_Dic->setWindowTitle("Collatinus : " + listeD.courant2 ()->nom());
    }
    else if (fen_Dic->isVisible()) fen_Dic->hide();
    else
    {
        actionExtra_dico->setChecked(true);
        extraDicVisible = true;
        fen_Dic->show();
        fen_Dic->setWindowTitle("Collatinus : " + listeD.courant2 ()->nom());
    }
}

void fenestra::ampliatioGlossarii ()
{
    // benchmark 1ère mesure
    qint64 tDebut = QDateTime::currentDateTime ().toMSecsSinceEpoch ();
    lexicum->ampliatio();
    action_ampliatio_glossarii->setEnabled(false);
    // benchmark 2ème mesure
    qDebug () << QDateTime::currentDateTime ().toMSecsSinceEpoch () - tDebut;
}

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
    if (extraDicVisible)
    {
        fen_dic->clear();
        fen_dic->setHtml(listeD.courant2 ()->page(lk, no));
        saisie_felix2->setText (lk.at (no));
        if (listeD.courant2 ()->estXml ())
        {
            AnteButton2->setText (listeD.courant2 ()->pgPrec ());
            PostButton2->setText (listeD.courant2 ()->pgSuiv ());
        }
        else
        {
            AnteButton2->setText (tr ("Retro"));
            PostButton2->setText (tr ("Porro"));
            labelLewis2->setText (QString::number (listeD.courant2 ()->noPageDjvu ()));
        }
        fen_dic->setTextCursor(QTextCursor(fen_dic->document()));
    }
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
        foreach (AnalyseMorpho * am, analyses)
        {
            Entree *e = am->entree ();
            requete << e->canonR ();
        }
        lexicum->deleteAnalyses (analyses);
    }
    if (requete.empty ()) requete << saisie_felix->text ();
    requete.removeDuplicates ();
    EditFelix->setFocus ();
    affiche_lemmes_dic (requete);
    saisie_felix->setFocus();
    saisie_felix->selectAll();
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
    /*
#ifdef Q_OS_MAC
// Pour singulariser le Mac
    QString chemin = qApp->applicationDirPath (); // .../Contents/MacOS
    chemin.chop(5); // Supprime MacOS
    chemin += "Resources/doc/index.html";
    QDesktopServices::openUrl(QUrl("file:" + chemin));
#else
*/
    QDesktopServices::openUrl(QUrl("file:" + qApp->applicationDirPath () + "/doc/index.html"));
// #endif
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

void fenestra::initPhrase ()
{
    syntaxe->creePhrase ("", -1, -1);
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

void fenestra::init_fen_Opt()
{
    fen_Opt->setWindowTitle(tr("Optiones accentus"));
    fen_Opt->resize(320, 120);
    groupBox = new QGroupBox(tr("Accentuer les mots en considérant \nune voyelle commune comme :"),fen_Opt);
    groupBox->setCheckable(true);
    groupBox->setChecked(false);
    radio1 = new QRadioButton(tr("longue"));
    radio2 = new QRadioButton(tr("brève"));
    radio3 = new QRadioButton(tr("ambiguë"));
    radio1->setChecked(true);
    checkBox = new QCheckBox(tr("Marquer la séparation entre les syllabes. "));
    checkBox->setChecked(true);
    vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addWidget(checkBox);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

}

int fenestra::lire_options()
{
    if (fen_Opt == NULL) return 7;
    if (!groupBox->isChecked()) return 0;
    int retour = 0;
    if (checkBox->isChecked()) retour = 4;
    if (radio1->isChecked()) retour += 1;
    if (radio2->isChecked()) retour += 2;
    if (radio3->isChecked()) retour += 3;
    return retour;
}

void fenestra::optionsAccent(bool vis)
{
//    FenetreOptions *fen_opt = new FenetreOptions(this);
//    fen_opt->show();
    if (fen_Opt == NULL)
    {
        fen_Opt = new QWidget(0);
        init_fen_Opt();

    }
    if (vis) fen_Opt->show();
    else if (fen_Opt->isVisible()) fen_Opt->hide();
    else
    {
        fen_Opt->show();
        actionOptiones_accentus->setChecked(true);
    }
}

void fenestra::createActions ()
{
    connect (actionOptiones_accentus, SIGNAL (toggled (bool)), this, SLOT (optionsAccent(bool)));
    connect (actionLire_fichier_hyphen, SIGNAL (triggered ()), this, SLOT (lireFichierHyphen()));
    connect (actionOter_les_accents, SIGNAL (triggered ()), this, SLOT (oteDiac()));
    connect(action_ampliatio_glossarii, SIGNAL(triggered()), this, SLOT(ampliatioGlossarii()));
    connect (actionTabula_Formae, SIGNAL (triggered ()), this, SLOT (tabulaFormae ()));
    connect(actionVerba_cognita, SIGNAL(toggled(bool)), this, SLOT(verbaCognita(bool)));
    connect(actionLancer_le_serveur, SIGNAL(toggled(bool)), this, SLOT(lancerServeur(bool)));
    connect(actionExtra_dico, SIGNAL(toggled(bool)), this, SLOT(extra_dico(bool)));
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
    connect(EditLatin, SIGNAL(textChanged()), this, SLOT(initPhrase ())); // actionVide_texte, SLOT(trigger()));
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
    connect(SaisieFlexion, SIGNAL(editingFinished ()), this, SLOT (affiche_flexion_saisie ()));
//  Actions liées au dico
    connect(saisie_felix, SIGNAL(returnPressed ()), this, SLOT (affiche_lemme_saisie ()));
    connect(BFlexion, SIGNAL(clicked ()), this, SLOT (affiche_flexion_saisie ()));
    connect(comboGlossaria, SIGNAL(currentIndexChanged(QString)), this, SLOT (change_glossarium (QString))); // nov
    connect(AnteButton, SIGNAL(clicked ()), this, SLOT (clicAnte ()));
    connect(PostButton, SIGNAL(clicked ()), this, SLOT (clicPost ()));
    // hyperliens dans la page dictionnaire
    connect(EditFelix, SIGNAL(anchorClicked (QUrl)), this, SLOT (affiche_lien (QUrl)));
    connect(felixButton, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie ()));
    connect(bDicoLitt, SIGNAL(clicked ()), this, SLOT (affiche_lemme_saisie_litt ()));
    // fin des actions liées au dico
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
    QApplication app (argc, argv);
#if (QT_VERSION < 0x050000)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    app.setStyle (new QPlastiqueStyle); // pour
#endif
#ifdef Q_OS_LINUX
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif
#ifndef QT_NO_TRANSLATION
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    translatorFileName += ".qm";
    QTranslator *translator = new QTranslator(&app);
    if (translator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(translator);
#endif
    // répertoire des ressources
    qsuia = app.applicationDirPath () + "/ressources/";
    QDir::setSearchPaths("donnees", QStringList(app.applicationDirPath () + "/ressources"));
    QCoreApplication::setOrganizationName("Collatinus");
    QCoreApplication::setOrganizationDomain("Collatinus.org");
    QCoreApplication::setApplicationName("Collatinus");
    fenestra f (argv[1]);
    f.show ();
    return app.exec();
}
