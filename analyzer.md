# Analyseur de Complexité — Extension VSCode (C++ & Python)

> Guide complet de création, étape par étape.

---

## Structure Finale du Projet

```
cpp-complexity-analyzer/
├── .vscode/
│   ├── extensions.json
│   ├── launch.json
│   ├── settings.json
│   └── tasks.json
├── node_modules/
├── src/
│   ├── analyzers/
│   │   ├── pythonAnalyzer.ts        ← Analyse AST Python (tree-sitter)
│   │   ├── cppAnalyzer.ts           ← Analyse AST C++ (tree-sitter)
│   │   └── complexityCalculator.ts  ← Logique commune de calcul Big O
│   ├── decorations/
│   │   ├── inlineDecorator.ts       ← Affichage inline "Complexité: O(n²)"
│   │   └── colorMapper.ts           ← Mapping complexité → couleur (du graphique)
│   ├── commands/
│   │   └── commands.ts              ← Commandes Command Palette
│   ├── test/
│   │   ├── extension.test.ts
│   │   ├── pythonAnalyzer.test.ts
│   │   ├── cppAnalyzer.test.ts
│   │   └── fixtures/
│   │       ├── sample.py            ← Fichiers de test Python
│   │       └── sample.cpp           ← Fichiers de test C++
│   └── extension.ts                 ← Point d'entrée de l'extension
├── .vscode-test.mjs
├── .vscodeignore
├── CHANGELOG.md
├── esbuild.js
├── eslint.config.mjs
├── package-lock.json
├── package.json
├── README.md
└── tsconfig.json
```

### Différences avec la structure initiale

La structure initiale (Image 1) contient le squelette généré par `yo code`. La structure finale ajoute :
- `src/analyzers/` — les 3 fichiers d'analyse (Python, C++, calcul commun)
- `src/decorations/` — les 2 fichiers d'affichage (inline + couleurs)
- `src/commands/` — les commandes de la Command Palette
- `src/test/fixtures/` — les fichiers exemples pour les tests
- `src/test/` — tests unitaires par module

---

## Table des Couleurs de Complexité

Basé sur le graphique Big O (Image 2) :

| Complexité   | Catégorie | Couleur de fond (rgba)             | Code hex approx. |
|-------------|-----------|-------------------------------------|-------------------|
| O(1)        | Excellent | `rgba(49, 180, 76, 0.15)`          | #31B44C (vert)    |
| O(log n)    | Excellent | `rgba(49, 180, 76, 0.15)`          | #31B44C (vert)    |
| O(n)        | Good      | `rgba(196, 212, 0, 0.15)`          | #C4D400 (jaune-vert) |
| O(n log n)  | Fair      | `rgba(255, 200, 0, 0.15)`          | #FFC800 (orange)  |
| O(n²)       | Bad       | `rgba(255, 106, 82, 0.15)`         | #FF6A52 (rouge-orange) |
| O(2ⁿ)       | Horrible  | `rgba(230, 57, 70, 0.15)`          | #E63946 (rouge)   |
| O(n!)       | Horrible  | `rgba(230, 57, 70, 0.15)`          | #E63946 (rouge)   |

> L'opacité de 0.15 est utilisée pour le fond de la ligne afin de ne pas rendre le code illisible. Le texte de l'annotation inline sera en couleur pleine.

---

## Étape 0 — Prérequis et Environnement

### 0.1 — Installer les outils nécessaires

- **Node.js** (v18+ recommandé) : [nodejs.org](https://nodejs.org)
- **npm** (inclus avec Node.js)
- **Visual Studio Code** (dernière version stable)
- **Git** (pour le versioning)

### 0.2 — Installer le générateur d'extension VSCode

```bash
npm install -g yo generator-code
```

### 0.3 — Vérifier les installations

```bash
node --version      # v18.x ou plus
npm --version       # 9.x ou plus
yo --version        # 5.x ou plus
code --version      # Dernière version stable
```

---

## Étape 1 — Générer le Squelette de l'Extension

### 1.1 — Lancer le générateur

```bash
yo code
```

### 1.2 — Répondre aux questions du générateur

| Question | Réponse |
|----------|---------|
| What type of extension? | `New Extension (TypeScript)` |
| What's the name? | `cpp-complexity-analyzer` |
| What's the identifier? | `cpp-complexity-analyzer` |
| What's the description? | `Analyse la complexité Big O des fonctions Python et C++` |
| Initialize a git repository? | `Yes` |
| Bundle with webpack/esbuild? | `esbuild` |
| Which package manager? | `npm` |

### 1.3 — Ouvrir le projet dans VSCode

```bash
cd cpp-complexity-analyzer
code .
```

### 1.4 — Vérifier que le squelette fonctionne

- Appuyer sur `F5` pour lancer l'Extension Development Host
- Dans la nouvelle fenêtre, ouvrir la Command Palette (`Ctrl+Shift+P`)
- Taper `Hello World` → un message devrait apparaître
- Fermer la fenêtre de test

### 1.5 — Comprendre les fichiers générés

| Fichier | Rôle |
|---------|------|
| `package.json` | Manifeste de l'extension (commandes, activation, dépendances) |
| `src/extension.ts` | Point d'entrée — fonctions `activate()` et `deactivate()` |
| `tsconfig.json` | Configuration TypeScript |
| `esbuild.js` | Configuration du bundler |
| `.vscode/launch.json` | Configuration de débogage (F5) |
| `.vscode/tasks.json` | Tâches de build |
| `src/test/extension.test.ts` | Tests de base |

---

## Étape 2 — Installer les Dépendances

### 2.1 — Installer tree-sitter et les grammaires

```bash
npm install web-tree-sitter
npm install tree-sitter-python tree-sitter-cpp
```

> **Pourquoi `web-tree-sitter` ?** C'est la version WebAssembly de tree-sitter. Elle fonctionne dans l'environnement Node.js de VSCode sans compilation native. Les grammaires `tree-sitter-python` et `tree-sitter-cpp` fournissent les fichiers `.wasm` nécessaires au parsing.

### 2.2 — Installer les types VSCode (déjà présents normalement)

```bash
npm install --save-dev @types/vscode
```

### 2.3 — Vérifier le `package.json`

Ouvrir `package.json` et vérifier que les dépendances sont bien listées :

```json
"dependencies": {
    "web-tree-sitter": "^0.24.x"
},
"devDependencies": {
    "@types/vscode": "^1.9x.x",
    ...
}
```

### 2.4 — Copier les fichiers WASM des grammaires

Les fichiers `.wasm` doivent être accessibles au runtime. Créer un script ou les copier manuellement :

```bash
mkdir -p out/wasm
cp node_modules/tree-sitter-python/tree-sitter-python.wasm out/wasm/
cp node_modules/tree-sitter-cpp/tree-sitter-cpp.wasm out/wasm/
```

> **Important** : Ajouter cette copie dans le script de build (`esbuild.js`) pour qu'elle soit automatique. Voir l'étape 3.5.

### 2.5 — Configurer le `.vscodeignore`

Ajouter les fichiers WASM pour qu'ils soient inclus dans le package de l'extension :

```
!out/wasm/**
```

---

## Étape 3 — Créer la Structure de Dossiers

### 3.1 — Créer les dossiers

```bash
mkdir -p src/analyzers
mkdir -p src/decorations
mkdir -p src/commands
mkdir -p src/test/fixtures
```

### 3.2 — Créer les fichiers vides

```bash
touch src/analyzers/pythonAnalyzer.ts
touch src/analyzers/cppAnalyzer.ts
touch src/analyzers/complexityCalculator.ts
touch src/decorations/inlineDecorator.ts
touch src/decorations/colorMapper.ts
touch src/commands/commands.ts
touch src/test/pythonAnalyzer.test.ts
touch src/test/cppAnalyzer.test.ts
```

### 3.3 — Créer les fichiers de test (fixtures)

Créer `src/test/fixtures/sample.py` :

```python
# O(1) - Constant
def get_first(arr):
    return arr[0]

# O(n) - Linéaire
def linear_search(arr, target):
    for item in arr:
        if item == target:
            return True
    return False

# O(n²) - Quadratique
def bubble_sort(arr):
    n = len(arr)
    for i in range(n):
        for j in range(n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
    return arr

# O(log n) - Logarithmique
def binary_search(arr, target):
    low, high = 0, len(arr) - 1
    while low <= high:
        mid = (low + high) // 2
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return -1
```

Créer `src/test/fixtures/sample.cpp` :

```cpp
#include <vector>
#include <algorithm>

// O(1) - Constant
int getFirst(std::vector<int>& arr) {
    return arr[0];
}

// O(n) - Linéaire
bool linearSearch(std::vector<int>& arr, int target) {
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] == target) return true;
    }
    return false;
}

// O(n²) - Quadratique
void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// O(log n) - Logarithmique
int binarySearch(std::vector<int>& arr, int target) {
    int low = 0, high = arr.size() - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
```

### 3.4 — Vérifier la structure

```bash
find src -type f | sort
```

Résultat attendu :

```
src/analyzers/complexityCalculator.ts
src/analyzers/cppAnalyzer.ts
src/analyzers/pythonAnalyzer.ts
src/commands/commands.ts
src/decorations/colorMapper.ts
src/decorations/inlineDecorator.ts
src/extension.ts
src/test/extension.test.ts
src/test/fixtures/sample.cpp
src/test/fixtures/sample.py
src/test/pythonAnalyzer.test.ts
src/test/cppAnalyzer.test.ts
```

### 3.5 — Mettre à jour `esbuild.js` pour copier les fichiers WASM

Ajouter à la fin du fichier `esbuild.js` un plugin ou un script `postbuild` qui copie les `.wasm` :

```javascript
const fs = require('fs');
const path = require('path');

// Après le build esbuild, copier les fichiers WASM
const wasmDir = path.join(__dirname, 'out', 'wasm');
if (!fs.existsSync(wasmDir)) {
    fs.mkdirSync(wasmDir, { recursive: true });
}

const grammars = ['tree-sitter-python', 'tree-sitter-cpp'];
for (const grammar of grammars) {
    const src = path.join(__dirname, 'node_modules', grammar, `${grammar}.wasm`);
    const dest = path.join(wasmDir, `${grammar}.wasm`);
    if (fs.existsSync(src)) {
        fs.copyFileSync(src, dest);
    }
}
```

---

## Étape 4 — Implémenter le Calculateur de Complexité

### 4.1 — `src/analyzers/complexityCalculator.ts`

Ce fichier contient la logique commune de calcul, indépendante du langage.

```typescript
/**
 * Représente le résultat d'analyse d'une fonction.
 */
export interface FunctionComplexity {
    name: string;           // Nom de la fonction
    startLine: number;      // Ligne de début (signature)
    endLine: number;        // Ligne de fin
    complexity: string;     // Notation Big O : "O(1)", "O(n)", "O(n²)", etc.
    level: ComplexityLevel; // Catégorie pour le mapping couleur
}

/**
 * Niveaux de complexité basés sur le graphique Big O.
 */
export enum ComplexityLevel {
    EXCELLENT = 'excellent', // O(1), O(log n)
    GOOD = 'good',           // O(n)
    FAIR = 'fair',           // O(n log n)
    BAD = 'bad',             // O(n²)
    HORRIBLE = 'horrible',   // O(2ⁿ), O(n!)
}

/**
 * Analyse la profondeur maximale d'imbrication de boucles
 * dans un sous-arbre AST tree-sitter.
 *
 * @param node - Noeud tree-sitter racine de la fonction
 * @param loopTypes - Noms des noeuds de boucle selon le langage
 * @returns La profondeur maximale de boucles imbriquées
 */
export function computeMaxLoopDepth(
    node: any,
    loopTypes: string[]
): number {
    let maxDepth = 0;

    function walk(current: any, depth: number): void {
        if (loopTypes.includes(current.type)) {
            depth++;
            maxDepth = Math.max(maxDepth, depth);
        }

        for (let i = 0; i < current.childCount; i++) {
            walk(current.child(i), depth);
        }
    }

    walk(node, 0);
    return maxDepth;
}

/**
 * Détecte si le corps de la fonction contient un pattern
 * de division par 2 typique de O(log n).
 *
 * Cherche des patterns comme :
 *  - mid = (low + high) / 2
 *  - low = mid + 1  ou  high = mid - 1
 *  - i /= 2  ou  i = i / 2
 */
export function hasHalvingPattern(
    node: any,
    sourceCode: string
): boolean {
    const bodyText = sourceCode.substring(node.startIndex, node.endIndex);
    // Patterns courants de division par 2
    const halvingPatterns = [
        /\/\s*2/,           // division par 2
        />>\s*1/,           // shift right (équivalent /2)
        /\/=\s*2/,          // division-assignation par 2
    ];
    return halvingPatterns.some(pattern => pattern.test(bodyText));
}

/**
 * Convertit la profondeur de boucle en notation Big O.
 *
 * @param maxLoopDepth - Profondeur maximale de boucles imbriquées
 * @param hasHalving - Présence d'un pattern de division par 2
 * @returns La notation Big O et le niveau de complexité
 */
export function depthToComplexity(
    maxLoopDepth: number,
    hasHalving: boolean
): { notation: string; level: ComplexityLevel } {
    if (maxLoopDepth === 0) {
        return { notation: 'O(1)', level: ComplexityLevel.EXCELLENT };
    }

    if (maxLoopDepth === 1 && hasHalving) {
        return { notation: 'O(log n)', level: ComplexityLevel.EXCELLENT };
    }

    if (maxLoopDepth === 1) {
        return { notation: 'O(n)', level: ComplexityLevel.GOOD };
    }

    if (maxLoopDepth === 2 && hasHalving) {
        return { notation: 'O(n log n)', level: ComplexityLevel.FAIR };
    }

    if (maxLoopDepth === 2) {
        return { notation: 'O(n²)', level: ComplexityLevel.BAD };
    }

    if (maxLoopDepth === 3) {
        return { notation: 'O(n³)', level: ComplexityLevel.HORRIBLE };
    }

    // 4+ boucles imbriquées
    return {
        notation: `O(n^${maxLoopDepth})`,
        level: ComplexityLevel.HORRIBLE
    };
}
```

### 4.2 — Explication de l'algorithme

L'algorithme d'analyse des boucles fonctionne comme suit :

1. **Parcours de l'AST** : On traverse récursivement l'arbre syntaxique de chaque fonction.
2. **Comptage de profondeur** : Chaque fois qu'on rencontre un noeud de type "boucle" (`for_statement`, `while_statement`, etc.), on incrémente la profondeur.
3. **Profondeur maximale** : On garde la profondeur maximale rencontrée.
4. **Détection de patterns** : On cherche des patterns de division par 2 dans le texte source pour distinguer O(log n) de O(n), et O(n log n) de O(n²).
5. **Conversion** : On convertit la profondeur + les patterns détectés en notation Big O.

### 4.3 — Limites connues de cette approche

- Ne détecte pas la récursion (sera ajouté dans une version future)
- Ne suit pas les appels de fonction (une boucle qui appelle une autre fonction avec une boucle ne sera pas comptée comme O(n²))
- Les patterns de halving sont détectés par regex, pas par analyse sémantique
- Les boucles qui ne dépendent pas de `n` seront quand même comptées (ex: une boucle `for i in range(10)` sera comptée comme O(n))

---

## Étape 5 — Implémenter l'Analyseur Python

### 5.1 — `src/analyzers/pythonAnalyzer.ts`

```typescript
import * as vscode from 'vscode';
import Parser from 'web-tree-sitter';
import * as path from 'path';
import {
    FunctionComplexity,
    computeMaxLoopDepth,
    hasHalvingPattern,
    depthToComplexity
} from './complexityCalculator';

// Types de noeuds de boucle en Python (tree-sitter)
const PYTHON_LOOP_TYPES = ['for_statement', 'while_statement'];

// Type de noeud de définition de fonction en Python
const PYTHON_FUNCTION_TYPE = 'function_definition';

let parser: Parser | null = null;

/**
 * Initialise le parser tree-sitter avec la grammaire Python.
 * Ne charge la grammaire qu'une seule fois.
 */
export async function initPythonParser(
    extensionPath: string
): Promise<Parser> {
    if (parser) return parser;

    await Parser.init();
    parser = new Parser();

    const wasmPath = path.join(extensionPath, 'out', 'wasm', 'tree-sitter-python.wasm');
    const language = await Parser.Language.load(wasmPath);
    parser.setLanguage(language);

    return parser;
}

/**
 * Analyse un fichier Python et retourne la complexité de chaque fonction.
 *
 * @param sourceCode - Le code source du fichier Python
 * @param extensionPath - Le chemin de l'extension (pour trouver le WASM)
 * @returns Liste des complexités par fonction
 */
export async function analyzePython(
    sourceCode: string,
    extensionPath: string
): Promise<FunctionComplexity[]> {
    const p = await initPythonParser(extensionPath);
    const tree = p.parse(sourceCode);
    const results: FunctionComplexity[] = [];

    // Parcourir l'arbre pour trouver toutes les définitions de fonctions
    function findFunctions(node: Parser.SyntaxNode): void {
        if (node.type === PYTHON_FUNCTION_TYPE) {
            const nameNode = node.childForFieldName('name');
            const name = nameNode ? nameNode.text : '<anonymous>';

            const maxDepth = computeMaxLoopDepth(node, PYTHON_LOOP_TYPES);
            const halving = hasHalvingPattern(node, sourceCode);
            const { notation, level } = depthToComplexity(maxDepth, halving);

            results.push({
                name,
                startLine: node.startPosition.row,   // 0-indexed
                endLine: node.endPosition.row,
                complexity: notation,
                level
            });
        }

        // Continuer la recherche dans les enfants
        // (pour trouver les fonctions imbriquées aussi)
        for (let i = 0; i < node.childCount; i++) {
            findFunctions(node.child(i)!);
        }
    }

    findFunctions(tree.rootNode);
    return results;
}
```

### 5.2 — Noeuds tree-sitter Python importants

| Noeud tree-sitter | Correspond à |
|-------------------|-------------|
| `function_definition` | `def ma_fonction():` |
| `for_statement` | `for x in iterable:` |
| `while_statement` | `while condition:` |
| `if_statement` | `if condition:` |
| `call` | `ma_fonction()` (appel de fonction) |

---

## Étape 6 — Implémenter l'Analyseur C++

### 6.1 — `src/analyzers/cppAnalyzer.ts`

```typescript
import Parser from 'web-tree-sitter';
import * as path from 'path';
import {
    FunctionComplexity,
    computeMaxLoopDepth,
    hasHalvingPattern,
    depthToComplexity
} from './complexityCalculator';

// Types de noeuds de boucle en C++ (tree-sitter)
const CPP_LOOP_TYPES = ['for_statement', 'while_statement', 'do_statement', 'for_range_loop'];

// Type de noeud de définition de fonction en C++
const CPP_FUNCTION_TYPE = 'function_definition';

let parser: Parser | null = null;

/**
 * Initialise le parser tree-sitter avec la grammaire C++.
 */
export async function initCppParser(
    extensionPath: string
): Promise<Parser> {
    if (parser) return parser;

    await Parser.init();
    parser = new Parser();

    const wasmPath = path.join(extensionPath, 'out', 'wasm', 'tree-sitter-cpp.wasm');
    const language = await Parser.Language.load(wasmPath);
    parser.setLanguage(language);

    return parser;
}

/**
 * Analyse un fichier C++ et retourne la complexité de chaque fonction.
 */
export async function analyzeCpp(
    sourceCode: string,
    extensionPath: string
): Promise<FunctionComplexity[]> {
    const p = await initCppParser(extensionPath);
    const tree = p.parse(sourceCode);
    const results: FunctionComplexity[] = [];

    function findFunctions(node: Parser.SyntaxNode): void {
        if (node.type === CPP_FUNCTION_TYPE) {
            // En C++, le nom peut être dans un 'declarator'
            const declarator = node.childForFieldName('declarator');
            let name = '<anonymous>';

            if (declarator) {
                // Le declarator peut être un function_declarator
                // qui contient lui-même un identifiant
                const nameNode = findIdentifier(declarator);
                if (nameNode) name = nameNode.text;
            }

            const maxDepth = computeMaxLoopDepth(node, CPP_LOOP_TYPES);
            const halving = hasHalvingPattern(node, sourceCode);
            const { notation, level } = depthToComplexity(maxDepth, halving);

            results.push({
                name,
                startLine: node.startPosition.row,
                endLine: node.endPosition.row,
                complexity: notation,
                level
            });
        }

        for (let i = 0; i < node.childCount; i++) {
            findFunctions(node.child(i)!);
        }
    }

    findFunctions(tree.rootNode);
    return results;
}

/**
 * Trouve récursivement le premier identifiant dans un declarator C++.
 * Nécessaire car la structure peut être :
 *   function_declarator → identifier
 *   function_declarator → qualified_identifier → identifier
 *   etc.
 */
function findIdentifier(node: Parser.SyntaxNode): Parser.SyntaxNode | null {
    if (node.type === 'identifier' || node.type === 'field_identifier') {
        return node;
    }
    for (let i = 0; i < node.childCount; i++) {
        const result = findIdentifier(node.child(i)!);
        if (result) return result;
    }
    return null;
}
```

### 6.2 — Noeuds tree-sitter C++ importants

| Noeud tree-sitter | Correspond à |
|-------------------|-------------|
| `function_definition` | `void maFonction() { ... }` |
| `for_statement` | `for (int i = 0; ...)` |
| `while_statement` | `while (condition) { ... }` |
| `do_statement` | `do { ... } while (condition);` |
| `for_range_loop` | `for (auto& x : container)` |

### 6.3 — Différences clés entre l'analyse Python et C++

| Aspect | Python | C++ |
|--------|--------|-----|
| Types de boucle | `for_statement`, `while_statement` | `for_statement`, `while_statement`, `do_statement`, `for_range_loop` |
| Accès au nom | Directement via `name` field | Via `declarator` → recherche récursive d'`identifier` |
| Fonctions imbriquées | Fréquentes (closures, décorateurs) | Rares (lambdas, fonctions locales C++14+) |

---

## Étape 7 — Implémenter le Mapping de Couleurs

### 7.1 — `src/decorations/colorMapper.ts`

```typescript
import { ComplexityLevel } from '../analyzers/complexityCalculator';

/**
 * Couleurs basées sur le graphique de complexité Big O.
 *
 * Fond : rgba avec opacité faible (0.15) pour ne pas gêner la lecture.
 * Texte : couleur pleine pour l'annotation inline.
 */
export interface ComplexityColors {
    backgroundColor: string;  // Fond de la ligne (rgba, opacité faible)
    textColor: string;         // Couleur du texte "Complexité: O(...)"
    borderColor: string;       // Couleur de la bordure gauche (optionnel)
}

const COLOR_MAP: Record<ComplexityLevel, ComplexityColors> = {
    [ComplexityLevel.EXCELLENT]: {
        backgroundColor: 'rgba(49, 180, 76, 0.15)',
        textColor: '#31B44C',
        borderColor: '#31B44C',
    },
    [ComplexityLevel.GOOD]: {
        backgroundColor: 'rgba(196, 212, 0, 0.15)',
        textColor: '#9BA800',
        borderColor: '#C4D400',
    },
    [ComplexityLevel.FAIR]: {
        backgroundColor: 'rgba(255, 200, 0, 0.15)',
        textColor: '#CC9900',
        borderColor: '#FFC800',
    },
    [ComplexityLevel.BAD]: {
        backgroundColor: 'rgba(255, 106, 82, 0.15)',
        textColor: '#FF6A52',
        borderColor: '#FF6A52',
    },
    [ComplexityLevel.HORRIBLE]: {
        backgroundColor: 'rgba(230, 57, 70, 0.15)',
        textColor: '#E63946',
        borderColor: '#E63946',
    },
};

/**
 * Retourne les couleurs associées à un niveau de complexité.
 */
export function getColorsForLevel(level: ComplexityLevel): ComplexityColors {
    return COLOR_MAP[level];
}
```

---

## Étape 8 — Implémenter l'Affichage Inline

### 8.1 — `src/decorations/inlineDecorator.ts`

```typescript
import * as vscode from 'vscode';
import { FunctionComplexity, ComplexityLevel } from '../analyzers/complexityCalculator';
import { getColorsForLevel } from './colorMapper';

/**
 * Stocke les types de décoration créés pour pouvoir les nettoyer.
 * On crée un type par niveau de complexité (pour les couleurs différentes).
 */
const decorationTypes: Map<ComplexityLevel, vscode.TextEditorDecorationType> = new Map();

/**
 * Crée (ou récupère du cache) un type de décoration pour un niveau donné.
 */
function getDecorationType(level: ComplexityLevel): vscode.TextEditorDecorationType {
    if (decorationTypes.has(level)) {
        return decorationTypes.get(level)!;
    }

    const colors = getColorsForLevel(level);

    const decorationType = vscode.window.createTextEditorDecorationType({
        // Fond de la ligne entière (signature de la fonction)
        backgroundColor: colors.backgroundColor,
        isWholeLine: true,
        // Bordure gauche colorée
        borderWidth: '0 0 0 3px',
        borderStyle: 'solid',
        borderColor: colors.borderColor,
        // Annotation inline après la ligne
        after: {
            margin: '0 0 0 1em',
            fontStyle: 'italic',
            fontWeight: 'normal',
        },
    });

    decorationTypes.set(level, decorationType);
    return decorationType;
}

/**
 * Applique les décorations sur l'éditeur actif.
 *
 * Pour chaque fonction analysée :
 * 1. Teinte la ligne de la signature avec la couleur du niveau
 * 2. Ajoute une annotation inline "Complexité: O(...)" après la signature
 */
export function applyDecorations(
    editor: vscode.TextEditor,
    results: FunctionComplexity[]
): void {
    // Nettoyer toutes les décorations existantes d'abord
    clearDecorations(editor);

    // Grouper les résultats par niveau de complexité
    const grouped = new Map<ComplexityLevel, vscode.DecorationOptions[]>();

    for (const result of results) {
        const level = result.level;
        if (!grouped.has(level)) {
            grouped.set(level, []);
        }

        const colors = getColorsForLevel(level);
        const line = result.startLine; // 0-indexed (tree-sitter)

        const range = new vscode.Range(
            new vscode.Position(line, 0),
            new vscode.Position(line, Number.MAX_SAFE_INTEGER)
        );

        grouped.get(level)!.push({
            range,
            renderOptions: {
                after: {
                    contentText: `  ← Complexité: ${result.complexity}`,
                    color: colors.textColor,
                },
            },
        });
    }

    // Appliquer les décorations groupées par niveau
    for (const [level, decorations] of grouped) {
        const decorationType = getDecorationType(level);
        editor.setDecorations(decorationType, decorations);
    }
}

/**
 * Supprime toutes les décorations de complexité de l'éditeur.
 */
export function clearDecorations(editor: vscode.TextEditor): void {
    for (const [, decorationType] of decorationTypes) {
        editor.setDecorations(decorationType, []);
    }
}

/**
 * Dispose tous les types de décoration (appelé à la désactivation).
 */
export function disposeDecorations(): void {
    for (const [, decorationType] of decorationTypes) {
        decorationType.dispose();
    }
    decorationTypes.clear();
}
```

### 8.2 — Ce que l'utilisateur verra

Pour une fonction Python comme :

```python
def bubble_sort(arr):           # ← cette ligne sera teintée en rouge-orange
    n = len(arr)                #    avec l'annotation "← Complexité: O(n²)"
    for i in range(n):
        for j in range(n - i - 1):
            ...
```

La ligne `def bubble_sort(arr):` aura :
- Un **fond rouge-orange** semi-transparent (`rgba(255, 106, 82, 0.15)`)
- Une **bordure gauche rouge-orange** de 3px
- Le texte **`← Complexité: O(n²)`** en italique rouge-orange après la signature

---

## Étape 9 — Implémenter les Commandes

### 9.1 — `src/commands/commands.ts`

```typescript
import * as vscode from 'vscode';

/**
 * Commande : Analyser le fichier courant manuellement.
 *
 * Déclenche l'analyse du fichier ouvert dans l'éditeur actif.
 * Utile quand l'analyse automatique est désactivée.
 */
export function registerAnalyzeCommand(
    context: vscode.ExtensionContext,
    analyzeCallback: () => Promise<void>
): void {
    const command = vscode.commands.registerCommand(
        'complexity-analyzer.analyzeCurrentFile',
        async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showWarningMessage(
                    'Aucun fichier ouvert à analyser.'
                );
                return;
            }

            const langId = editor.document.languageId;
            if (langId !== 'python' && langId !== 'cpp') {
                vscode.window.showWarningMessage(
                    `Langage "${langId}" non supporté. Seuls Python et C++ sont supportés.`
                );
                return;
            }

            await analyzeCallback();
            vscode.window.showInformationMessage(
                `Analyse de complexité terminée pour ${editor.document.fileName}`
            );
        }
    );

    context.subscriptions.push(command);
}

/**
 * Commande : Activer/Désactiver l'analyse automatique.
 *
 * Toggle un paramètre de configuration qui contrôle
 * si l'analyse se relance à chaque sauvegarde.
 */
export function registerToggleAutoAnalyzeCommand(
    context: vscode.ExtensionContext
): void {
    const command = vscode.commands.registerCommand(
        'complexity-analyzer.toggleAutoAnalyze',
        async () => {
            const config = vscode.workspace.getConfiguration('complexityAnalyzer');
            const current = config.get<boolean>('autoAnalyze', true);
            await config.update('autoAnalyze', !current, vscode.ConfigurationTarget.Global);

            const state = !current ? 'activée' : 'désactivée';
            vscode.window.showInformationMessage(
                `Analyse automatique ${state}.`
            );
        }
    );

    context.subscriptions.push(command);
}

/**
 * Commande : Configurer les seuils de couleurs.
 *
 * Ouvre les settings de l'extension pour que l'utilisateur
 * puisse ajuster les couleurs et seuils.
 */
export function registerConfigureColorsCommand(
    context: vscode.ExtensionContext
): void {
    const command = vscode.commands.registerCommand(
        'complexity-analyzer.configureColors',
        () => {
            vscode.commands.executeCommand(
                'workbench.action.openSettings',
                'complexityAnalyzer.colors'
            );
        }
    );

    context.subscriptions.push(command);
}
```

---

## Étape 10 — Mettre à Jour `package.json`

### 10.1 — Déclarer les commandes

Dans `package.json`, remplacer la section `contributes` existante :

```json
"contributes": {
    "commands": [
        {
            "command": "complexity-analyzer.analyzeCurrentFile",
            "title": "Analyser la complexité du fichier courant",
            "category": "Complexity Analyzer"
        },
        {
            "command": "complexity-analyzer.toggleAutoAnalyze",
            "title": "Activer/Désactiver l'analyse automatique",
            "category": "Complexity Analyzer"
        },
        {
            "command": "complexity-analyzer.configureColors",
            "title": "Configurer les couleurs de complexité",
            "category": "Complexity Analyzer"
        }
    ],
    "configuration": {
        "title": "Complexity Analyzer",
        "properties": {
            "complexityAnalyzer.autoAnalyze": {
                "type": "boolean",
                "default": true,
                "description": "Analyser automatiquement à chaque sauvegarde."
            },
            "complexityAnalyzer.colors.excellent": {
                "type": "string",
                "default": "#31B44C",
                "description": "Couleur pour O(1) et O(log n)"
            },
            "complexityAnalyzer.colors.good": {
                "type": "string",
                "default": "#C4D400",
                "description": "Couleur pour O(n)"
            },
            "complexityAnalyzer.colors.fair": {
                "type": "string",
                "default": "#FFC800",
                "description": "Couleur pour O(n log n)"
            },
            "complexityAnalyzer.colors.bad": {
                "type": "string",
                "default": "#FF6A52",
                "description": "Couleur pour O(n²)"
            },
            "complexityAnalyzer.colors.horrible": {
                "type": "string",
                "default": "#E63946",
                "description": "Couleur pour O(2ⁿ) et O(n!)"
            }
        }
    }
}
```

### 10.2 — Déclarer les événements d'activation

Remplacer la section `activationEvents` :

```json
"activationEvents": [
    "onLanguage:python",
    "onLanguage:cpp"
]
```

> Cela signifie que l'extension se charge automatiquement quand un fichier Python ou C++ est ouvert. Pas besoin de l'activer manuellement.

### 10.3 — Mettre à jour les métadonnées

```json
"displayName": "Complexity Analyzer (C++ & Python)",
"description": "Analyse et affiche la complexité Big O de vos fonctions Python et C++ directement dans l'éditeur.",
"categories": ["Programming Languages", "Linters"],
"keywords": ["complexity", "big-o", "python", "cpp", "performance"],
"engines": {
    "vscode": "^1.90.0"
}
```

---

## Étape 11 — Implémenter le Point d'Entrée

### 11.1 — `src/extension.ts`

```typescript
import * as vscode from 'vscode';
import { analyzePython } from './analyzers/pythonAnalyzer';
import { analyzeCpp } from './analyzers/cppAnalyzer';
import { FunctionComplexity } from './analyzers/complexityCalculator';
import { applyDecorations, clearDecorations, disposeDecorations } from './decorations/inlineDecorator';
import {
    registerAnalyzeCommand,
    registerToggleAutoAnalyzeCommand,
    registerConfigureColorsCommand
} from './commands/commands';

/**
 * Fonction principale d'analyse.
 * Détecte le langage et délègue à l'analyseur approprié.
 */
async function analyzeActiveEditor(
    extensionPath: string
): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) return;

    const document = editor.document;
    const languageId = document.languageId;
    const sourceCode = document.getText();

    let results: FunctionComplexity[] = [];

    try {
        if (languageId === 'python') {
            results = await analyzePython(sourceCode, extensionPath);
        } else if (languageId === 'cpp') {
            results = await analyzeCpp(sourceCode, extensionPath);
        } else {
            // Langage non supporté — on ne fait rien
            return;
        }
    } catch (error) {
        console.error('Erreur lors de l\'analyse:', error);
        vscode.window.showErrorMessage(
            `Erreur lors de l'analyse de complexité: ${error}`
        );
        return;
    }

    // Appliquer les décorations
    applyDecorations(editor, results);
}

/**
 * Appelée quand l'extension est activée.
 */
export function activate(context: vscode.ExtensionContext): void {
    console.log('Complexity Analyzer activé');

    const extensionPath = context.extensionPath;

    // Fonction wrapper pour les commandes
    const runAnalysis = () => analyzeActiveEditor(extensionPath);

    // --- Enregistrer les commandes ---
    registerAnalyzeCommand(context, runAnalysis);
    registerToggleAutoAnalyzeCommand(context);
    registerConfigureColorsCommand(context);

    // --- Analyse à l'ouverture d'un fichier supporté ---
    if (vscode.window.activeTextEditor) {
        runAnalysis();
    }

    // --- Analyse quand on change d'éditeur actif ---
    const onEditorChange = vscode.window.onDidChangeActiveTextEditor(
        (editor) => {
            if (editor) {
                const lang = editor.document.languageId;
                if (lang === 'python' || lang === 'cpp') {
                    runAnalysis();
                } else {
                    clearDecorations(editor);
                }
            }
        }
    );
    context.subscriptions.push(onEditorChange);

    // --- Analyse à chaque sauvegarde ---
    const onSave = vscode.workspace.onDidSaveTextDocument(
        (document) => {
            const config = vscode.workspace.getConfiguration('complexityAnalyzer');
            const autoAnalyze = config.get<boolean>('autoAnalyze', true);

            if (!autoAnalyze) return;

            const lang = document.languageId;
            if (lang === 'python' || lang === 'cpp') {
                runAnalysis();
            }
        }
    );
    context.subscriptions.push(onSave);
}

/**
 * Appelée quand l'extension est désactivée.
 */
export function deactivate(): void {
    disposeDecorations();
    console.log('Complexity Analyzer désactivé');
}
```

### 11.2 — Flux d'exécution complet

```
Ouverture d'un fichier .py ou .cpp
          │
          ▼
    activate() est appelé
          │
          ▼
    analyzeActiveEditor()
          │
          ├── languageId === 'python' → analyzePython()
          │                                    │
          │                                    ▼
          │                          tree-sitter parse → AST
          │                                    │
          │                                    ▼
          │                          findFunctions() → parcours récursif
          │                                    │
          │                                    ▼
          │                          computeMaxLoopDepth() pour chaque fonction
          │                                    │
          │                                    ▼
          │                          depthToComplexity() → notation Big O
          │
          ├── languageId === 'cpp' → analyzeCpp()  (même logique)
          │
          ▼
    applyDecorations()
          │
          ├── Grouper par ComplexityLevel
          ├── Créer les DecorationTypes (fond + bordure + couleur)
          └── Appliquer sur l'éditeur (fond teinté + annotation inline)
```

---

## Étape 12 — Configurer le Build et le Bundling

### 12.1 — Vérifier `tsconfig.json`

S'assurer que les options suivantes sont présentes :

```json
{
    "compilerOptions": {
        "module": "commonjs",
        "target": "ES2021",
        "outDir": "out",
        "lib": ["ES2021"],
        "sourceMap": true,
        "rootDir": "src",
        "strict": true,
        "esModuleInterop": true,
        "resolveJsonModule": true
    },
    "exclude": ["node_modules", ".vscode-test"]
}
```

### 12.2 — Mettre à jour `esbuild.js`

S'assurer que `web-tree-sitter` est correctement bundlé. Ajouter `web-tree-sitter` aux externals si nécessaire, ou configurer le bundler pour le résoudre correctement.

Points importants :
- Les fichiers `.wasm` ne peuvent pas être bundlés par esbuild — ils doivent être copiés séparément (voir étape 2.4)
- `web-tree-sitter` utilise des imports dynamiques pour charger les `.wasm`

### 12.3 — Ajouter un script npm de build complet

Dans `package.json`, ajouter/modifier les scripts :

```json
"scripts": {
    "vscode:prepublish": "npm run package",
    "compile": "npm run check-types && npm run lint && node esbuild.js",
    "watch": "node esbuild.js --watch",
    "package": "npm run check-types && npm run lint && node esbuild.js --production",
    "check-types": "tsc --noEmit",
    "lint": "eslint src",
    "test": "vscode-test",
    "copy-wasm": "mkdir -p out/wasm && cp node_modules/tree-sitter-python/tree-sitter-python.wasm out/wasm/ && cp node_modules/tree-sitter-cpp/tree-sitter-cpp.wasm out/wasm/",
    "prebuild": "npm run copy-wasm"
}
```

### 12.4 — Compiler et vérifier

```bash
npm run compile
```

Vérifier qu'il n'y a pas d'erreurs TypeScript. Si oui, les corriger avant de continuer.

---

## Étape 13 — Écrire les Tests

### 13.1 — `src/test/pythonAnalyzer.test.ts`

```typescript
import * as assert from 'assert';
import * as path from 'path';
import * as fs from 'fs';
import { analyzePython } from '../analyzers/pythonAnalyzer';

suite('Python Analyzer Tests', () => {
    const extensionPath = path.resolve(__dirname, '../../');
    const fixturesPath = path.join(__dirname, 'fixtures');

    test('Détecte O(1) pour une fonction sans boucle', async () => {
        const code = `
def get_first(arr):
    return arr[0]
`;
        const results = await analyzePython(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(1)');
        assert.strictEqual(results[0].name, 'get_first');
    });

    test('Détecte O(n) pour une boucle simple', async () => {
        const code = `
def linear_search(arr, target):
    for item in arr:
        if item == target:
            return True
    return False
`;
        const results = await analyzePython(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(n)');
    });

    test('Détecte O(n²) pour deux boucles imbriquées', async () => {
        const code = `
def bubble_sort(arr):
    n = len(arr)
    for i in range(n):
        for j in range(n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
`;
        const results = await analyzePython(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(n²)');
    });

    test('Détecte O(log n) avec pattern de division', async () => {
        const code = `
def binary_search(arr, target):
    low, high = 0, len(arr) - 1
    while low <= high:
        mid = (low + high) / 2
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return -1
`;
        const results = await analyzePython(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(log n)');
    });

    test('Détecte plusieurs fonctions dans un fichier', async () => {
        const samplePath = path.join(fixturesPath, 'sample.py');
        const code = fs.readFileSync(samplePath, 'utf-8');
        const results = await analyzePython(code, extensionPath);
        assert.ok(results.length >= 4, `Attendu >= 4 fonctions, obtenu ${results.length}`);
    });
});
```

### 13.2 — `src/test/cppAnalyzer.test.ts`

```typescript
import * as assert from 'assert';
import * as path from 'path';
import * as fs from 'fs';
import { analyzeCpp } from '../analyzers/cppAnalyzer';

suite('C++ Analyzer Tests', () => {
    const extensionPath = path.resolve(__dirname, '../../');
    const fixturesPath = path.join(__dirname, 'fixtures');

    test('Détecte O(1) pour une fonction sans boucle', async () => {
        const code = `
int getFirst(int arr[], int size) {
    return arr[0];
}
`;
        const results = await analyzeCpp(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(1)');
    });

    test('Détecte O(n) pour une boucle for simple', async () => {
        const code = `
bool linearSearch(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) return true;
    }
    return false;
}
`;
        const results = await analyzeCpp(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(n)');
    });

    test('Détecte O(n²) pour deux boucles imbriquées', async () => {
        const code = `
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
`;
        const results = await analyzeCpp(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(n²)');
    });

    test('Détecte O(log n) avec division par 2', async () => {
        const code = `
int binarySearch(int arr[], int size, int target) {
    int low = 0, high = size - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}
`;
        const results = await analyzeCpp(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(log n)');
    });

    test('Détecte les range-based for loops', async () => {
        const code = `
int sum(std::vector<int>& v) {
    int total = 0;
    for (auto& x : v) {
        total += x;
    }
    return total;
}
`;
        const results = await analyzeCpp(code, extensionPath);
        assert.strictEqual(results.length, 1);
        assert.strictEqual(results[0].complexity, 'O(n)');
    });
});
```

### 13.3 — Lancer les tests

```bash
npm test
```

---

## Étape 14 — Tester Manuellement dans VSCode

### 14.1 — Lancer l'Extension Development Host

1. Ouvrir le projet dans VSCode
2. Appuyer sur `F5` (ou Run → Start Debugging)
3. Une nouvelle fenêtre VSCode s'ouvre avec l'extension chargée

### 14.2 — Tester avec un fichier Python

1. Dans la fenêtre de test, créer un fichier `test.py`
2. Écrire quelques fonctions avec différentes complexités
3. Sauvegarder le fichier
4. Vérifier que les annotations apparaissent sur les lignes de définition
5. Vérifier que les couleurs correspondent au niveau de complexité

### 14.3 — Tester avec un fichier C++

1. Créer un fichier `test.cpp`
2. Écrire quelques fonctions
3. Sauvegarder et vérifier les annotations

### 14.4 — Tester les commandes

1. Ouvrir la Command Palette (`Ctrl+Shift+P`)
2. Taper "Complexity" — les 3 commandes devraient apparaître
3. Tester chaque commande :
   - `Analyser la complexité du fichier courant` → déclenche l'analyse
   - `Activer/Désactiver l'analyse automatique` → toggle le setting
   - `Configurer les couleurs de complexité` → ouvre les settings

### 14.5 — Tester le changement d'éditeur

1. Ouvrir un fichier `.py` et un fichier `.cpp`
2. Basculer entre les deux onglets
3. Vérifier que les décorations se mettent à jour correctement
4. Ouvrir un fichier `.js` (non supporté) → les décorations doivent disparaître

### 14.6 — Tester la sauvegarde automatique

1. Modifier une fonction (ajouter une boucle imbriquée)
2. Sauvegarder (`Ctrl+S`)
3. Vérifier que l'annotation se met à jour automatiquement

---

## Étape 15 — Debugging et Résolution de Problèmes

### 15.1 — Les annotations n'apparaissent pas

**Vérifications** :
- Ouvrir la console de débogage (Help → Toggle Developer Tools → Console)
- Chercher les erreurs liées à `Complexity Analyzer`
- Vérifier que les fichiers `.wasm` sont bien dans `out/wasm/`
- Vérifier que `activationEvents` contient `onLanguage:python` et `onLanguage:cpp`

### 15.2 — Erreur de chargement WASM

**Cause probable** : Le chemin vers les fichiers `.wasm` est incorrect.
**Solution** : Vérifier que `extensionPath` pointe bien vers la racine de l'extension, et que les `.wasm` sont dans `out/wasm/` relativement à cette racine.

### 15.3 — Le parsing échoue silencieusement

**Cause probable** : tree-sitter ne reconnaît pas certaines syntaxes.
**Solution** : Ajouter des `try/catch` autour du parsing et logger les erreurs. Tester avec des fichiers simples d'abord.

### 15.4 — Les couleurs ne sont pas visibles

**Cause probable** : Le thème VSCode actuel a un fond qui rend le `rgba` invisible.
**Solution** : Augmenter l'opacité dans `colorMapper.ts` (de 0.15 à 0.25) ou tester avec un thème différent.

---

## Étape 16 — Polir l'Extension

### 16.1 — Ajouter une icône

1. Créer ou trouver une icône 128x128 PNG
2. La placer à la racine du projet : `icon.png`
3. Ajouter dans `package.json` :

```json
"icon": "icon.png"
```

### 16.2 — Rédiger le README.md

Le `README.md` est ce qui s'affiche sur le Marketplace. Inclure :
- Description de l'extension
- Capture d'écran montrant les annotations
- Langages supportés
- Commandes disponibles
- Configuration
- Limites connues

### 16.3 — Rédiger le CHANGELOG.md

```markdown
# Changelog

## [0.1.0] - 2026-XX-XX
### Added
- Analyse de complexité Big O pour Python et C++
- Annotations inline avec couleurs basées sur le graphique Big O
- Analyse automatique à la sauvegarde
- 3 commandes dans la Command Palette
- Support de tree-sitter pour le parsing AST
```

### 16.4 — Mettre à jour `.vscodeignore`

S'assurer que seuls les fichiers nécessaires sont dans le package :

```
.vscode/**
.vscode-test/**
src/**
node_modules/**
!node_modules/web-tree-sitter/**
!out/**
.gitignore
tsconfig.json
esbuild.js
eslint.config.mjs
```

---

## Étape 17 — Packager l'Extension

### 17.1 — Installer vsce

```bash
npm install -g @vscode/vsce
```

### 17.2 — Créer le package VSIX

```bash
vsce package
```

Cela génère un fichier `cpp-complexity-analyzer-0.1.0.vsix`.

### 17.3 — Tester l'installation du VSIX

1. Dans VSCode : Extensions → `...` → Install from VSIX
2. Sélectionner le fichier `.vsix`
3. Tester que tout fonctionne comme en mode développement

---

## Étape 18 — Publier sur le Marketplace (Optionnel)

### 18.1 — Créer un compte publisher

1. Aller sur [Azure DevOps](https://dev.azure.com)
2. Créer une organisation
3. Générer un Personal Access Token (PAT) avec le scope `Marketplace (Manage)`

### 18.2 — Créer un publisher

```bash
vsce create-publisher <ton-nom>
```

### 18.3 — Ajouter le publisher dans package.json

```json
"publisher": "<ton-nom>"
```

### 18.4 — Publier

```bash
vsce publish
```

---

## Récapitulatif des Fichiers et leur Rôle

| Fichier | Lignes approx. | Rôle |
|---------|---------------|------|
| `src/extension.ts` | ~80 | Point d'entrée, orchestration, événements |
| `src/analyzers/complexityCalculator.ts` | ~100 | Logique commune : parcours AST, calcul Big O |
| `src/analyzers/pythonAnalyzer.ts` | ~60 | Init tree-sitter Python, extraction des fonctions |
| `src/analyzers/cppAnalyzer.ts` | ~80 | Init tree-sitter C++, extraction des fonctions |
| `src/decorations/colorMapper.ts` | ~50 | Mapping complexité → couleurs (du graphique) |
| `src/decorations/inlineDecorator.ts` | ~90 | Création et application des décorations VSCode |
| `src/commands/commands.ts` | ~70 | 3 commandes Command Palette |
| `src/test/*.test.ts` | ~100 chacun | Tests unitaires |
| `package.json` | ~80 | Manifeste, commandes, configuration, activation |

**Total estimé** : ~800–900 lignes de code TypeScript (hors tests et config).

---

## Ordre d'Implémentation Recommandé

Pour avancer de manière incrémentale et testable :

1. **Étapes 0–3** : Setup, génération, dépendances, structure → ~30 min
2. **Étape 4** : complexityCalculator.ts → testable isolément → ~45 min
3. **Étape 5** : pythonAnalyzer.ts → tester avec les fixtures → ~30 min
4. **Étape 7** : colorMapper.ts → simple, rapide → ~15 min
5. **Étape 8** : inlineDecorator.ts → premier résultat visuel ! → ~45 min
6. **Étape 11** : extension.ts → tout relier ensemble → ~30 min
7. **Étape 10** : package.json → déclarer les commandes → ~15 min
8. **Étape 14** : Test manuel dans VSCode → vérifier le visuel → ~30 min
9. **Étape 6** : cppAnalyzer.ts → ajouter le support C++ → ~30 min
10. **Étape 9** : commands.ts → ajouter les commandes → ~20 min
11. **Étapes 13, 15, 16** : Tests, debugging, polish → ~1h
12. **Étapes 17–18** : Packaging et publication → ~30 min

**Temps total estimé** : ~6–8 heures pour un développeur familier avec TypeScript.
