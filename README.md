# Collatinus

## Version expérimentale de Collatinus

En cours de développement par Philippe Verkerk, d'après les suggestions de divers utilisateurs (Octave Boczkowski, Jan Bart et Élie Roux).

### Nouveautés

- Extension du lexique par traitement **automatique**, donc **imparfait**, des dictionnaires numériques (un grand merci à Gérard Gréco qui m'a confié son Gaffiot avant sa publication).
- Possibilité de consulter simultanément deux dictionnaires.
- Colorisation du texte en fonction d'une liste de mots connus (par les élèves).
- Indication de l'accentuation et des césures dans un texte.

### Avertissement

Les ressources pour cette version 10.3alpha **ne sont pas** compatibles à 100% avec Collatinus 10.2. On les trouvera dans la branche correspondante du dépôt Github [collatinus-data](https://github.com/biblissima/collatinus-data).

## Présentation

Ce dépôt contient les sources de [Collatinus](http://outils.biblissima.fr/collatinus/), un logiciel libre et gratuit pour la **lemmatisation et l'analyse morphologique de textes latins**.

Collatinus est développé par Yves Ouvrard et Philippe Verkerk.

**Page de présentation et téléchargement sur le site Biblissima** : [http://outils.biblissima.fr/collatinus/](http://outils.biblissima.fr/collatinus/) (exécutables disponibles pour Mac OS, GNU/Linux et Windows).

Collatinus existe aussi en **version web** :
- Démo sur le site Biblissima : [http://outils.biblissima.fr/collatinus-web/](http://outils.biblissima.fr/collatinus-web)
- Collatinus-web sur Github : [collatinus-web-daemon](https://github.com/biblissima/collatinus-web-daemon) et [collatinus-web-ui](https://github.com/biblissima/collatinus-web-ui)

### Principales fonctionnalités

- lemmatisation de mots latins ou d'un texte latin entier,
- traduction des lemmes grâce aux dictionnaires de latin incorporés dans l'application,
- affichage des quantités (durée longue ou brève des syllabes) et des flexions (déclinaison ou conjugaison).

## Installation

Collatinus a été écrit avec **Qt**. Le fichier du projet Qt, `collatinus.pro`, devra probablement être adapté à l'environnement local. Nous le donnons ici à titre d'exemple (peut-être même qu'il fonctionnera).

### Ajout des données

Pour que le programme Collatinus fonctionne, il faut lui fournir des **fichiers de données**. Ces fichiers sont rangés dans un répertoire `ressources/` que l'on trouvera dans le dépôt Github [collatinus-data](https://github.com/biblissima/collatinus-data). Les fichiers volumineux des dictionnaires (*.xml ou *.djvu) ne sont pas inclus dans le dépôt : ils doivent être téléchargés directement depuis la page Collatinus du site Biblissima (section Téléchargements : http://outils.biblissima.fr/collatinus/#telechargements)

Selon le système d'exploitation utilisé, le dossier `ressources/` devra être rangé à un endroit précis. Pour Windows et Linux, il doit se trouver au même niveau que l'exécutable. Pour Mac OSX, tout se passe dans le paquet `Collatinus.app` (ou tout autre nom que l'on souhaite lui donner) et le dossier `ressources/` doit être placé dans le répertoire `Contents/MacOS/` (à côté de l'exécutable ; cet emplacement n'est probablement pas conforme aux directives d'Apple, et pourrait être modifié dans une version ultérieure). 
Pour que ces données soient adaptées à la version bureau de Collatinus, il convient de créer, dans ce répertoire `ressources/`, un sous-répertoire `dicos/` dans lequel seront rangés les fichiers relatifs aux dictionnaires (*.cfg, *.idx, *.css et *.xsl) ainsi que les dictionnaires eux-mêmes.

Le **manuel d'aide** de Collatinus est disponible dans le dépôt [collatinus-data](https://github.com/biblissima/collatinus-data) (voir le dossier `doc/`; celui-ci devra être laissé à sa place à côté du dossier `ressources/` pour être inclu dans le programme).


## Licence

Ce programme est mis à disposition par Yves Ouvrard et Philippe Verkerk sous licence [GNU GPL v3](http://www.gnu.org/licenses/gpl.html).

## Crédits

**Remerciements** : William Whitaker †, Jose Luis Redrejo, Georges Khaznadar, Matthias Bussonier, Gérard Jeanneau, Philippe Verkerk, Jean-Paul Woitrain, Philipp Roelli, Perseus Digital Library.
