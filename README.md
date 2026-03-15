# Complexity Analyzer — VSCode Extension (C++ & Python)

[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](https://github.com/Matteodt974/cpp-complexity-analyzer/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/Matteodt974/cpp-complexity-analyzer/actions)
[![Language](https://img.shields.io/badge/language-TypeScript-blue.svg)](https://www.typescriptlang.org/)
[![VSCode](https://img.shields.io/badge/VSCode-%3E%3D1.90.0-blueviolet.svg)](https://code.visualstudio.com/)

A Visual Studio Code extension that analyzes and displays the **Big O time complexity** of your Python and C++ functions directly in the editor. Built for developers who need a quick, visual way to evaluate the complexity of algorithm implementations — especially in **ML/AI** workflows where performance matters.

![Demo](docs/demo.gif)
<!-- Replace with an actual screenshot or gif once available -->

## Motivation

When working on ML/AI projects, algorithm efficiency is critical. Whether you're implementing a custom data preprocessing pipeline, a search algorithm, or optimizing model inference code, knowing the time complexity of your functions at a glance saves time and prevents performance bottlenecks.

This extension was born out of a personal need to **quickly validate algorithmic choices** during ML development without having to mentally trace loop depths every time.

## How It Works

The extension uses [tree-sitter](https://tree-sitter.github.io/tree-sitter/) to parse your code into an Abstract Syntax Tree (AST), then analyzes the nesting depth of loops within each function to estimate the Big O complexity.

**Analysis pipeline:**

```
Source code → tree-sitter AST → Loop depth analysis → Big O estimation → Inline decoration
```

**What it detects:**

| Pattern | Detected Complexity |
|---------|-------------------|
| No loops | O(1) |
| Single loop with halving (e.g. binary search) | O(log n) |
| Single loop | O(n) |
| Two nested loops with halving | O(n log n) |
| Two nested loops | O(n²) |
| Three nested loops | O(n³) |
| Deeper nesting | O(n^k) |

**Color coding** (based on the classic Big O complexity chart):

| Complexity | Color | Category |
|-----------|-------|----------|
| O(1), O(log n) | 🟢 Green | Excellent |
| O(n) | 🟡 Yellow-green | Good |
| O(n log n) | 🟠 Orange | Fair |
| O(n²) | 🔴 Red-orange | Bad |
| O(n³)+ | 🔴 Red | Horrible |

## Features

- **Inline annotations** — Complexity displayed right after the function signature
- **Color-coded line highlighting** — Function definition line is tinted based on complexity level
- **Auto-analysis on save** — Results update every time you save the file
- **Dual language support** — Python and C++ with automatic language detection
- **Command Palette commands** — Manual analysis, toggle auto-analysis, configure colors
- **Configurable colors** — Customize the color thresholds in VSCode settings

## Project Structure

```
cpp-complexity-analyzer/
├── src/
│   ├── analyzers/
│   │   ├── pythonAnalyzer.ts         # Python AST analysis via tree-sitter
│   │   ├── cppAnalyzer.ts            # C++ AST analysis via tree-sitter
│   │   └── complexityCalculator.ts   # Shared Big O computation logic
│   ├── decorations/
│   │   ├── inlineDecorator.ts        # VSCode decoration API integration
│   │   └── colorMapper.ts            # Complexity level → color mapping
│   ├── commands/
│   │   └── commands.ts               # Command Palette commands
│   ├── test/
│   │   ├── extension.test.ts
│   │   ├── pythonAnalyzer.test.ts
│   │   ├── cppAnalyzer.test.ts
│   │   └── fixtures/
│   │       ├── sample.py
│   │       └── sample.cpp
│   └── extension.ts                  # Extension entry point
├── out/
│   └── wasm/                         # tree-sitter grammar binaries
├── package.json
├── tsconfig.json
├── esbuild.js
└── README.md
```

## Getting Started

### Prerequisites

- [Node.js](https://nodejs.org/) v18+
- [Visual Studio Code](https://code.visualstudio.com/) v1.90+
- npm (included with Node.js)

### Installation (Development)

```bash
# Clone the repository
git clone https://github.com/Matteodt974/cpp-complexity-analyzer.git
cd cpp-complexity-analyzer

# Install dependencies
npm install

# Copy tree-sitter WASM grammars
npm run copy-wasm

# Compile
npm run compile
```

### Running in Development

1. Open the project in VSCode
2. Press `F5` to launch the Extension Development Host
3. Open any `.py` or `.cpp` file in the new window
4. Save the file — complexity annotations should appear

### Running Tests

```bash
npm test
```

## Building & Packaging

```bash
# Install vsce if not already installed
npm install -g @vscode/vsce

# Package as VSIX
vsce package
```

This produces a `cpp-complexity-analyzer-x.x.x.vsix` file that can be installed in any VSCode instance.

## Known Limitations

- **No recursion detection** — Recursive functions are reported as O(1) if they contain no loops. Planned for a future version.
- **No cross-function analysis** — If function `f()` calls `g()` inside a loop and `g()` is O(n), the extension won't detect that `f()` is O(n²). It only analyzes each function in isolation.
- **Heuristic-based halving detection** — The O(log n) detection uses regex patterns (e.g., `/ 2`, `>> 1`) which may produce false positives or miss non-standard patterns.
- **Constant loops counted as O(n)** — A loop like `for i in range(10)` is reported as O(n) since the analysis doesn't evaluate bounds.

## Roadmap

- [ ] Recursive function analysis (Phase 2 — AST-based recursion pattern detection)
- [ ] Cross-function call graph analysis
- [ ] Space complexity estimation
- [ ] Support for additional languages (Java, Rust)
- [ ] Sidebar panel with full complexity report
- [ ] Export complexity report as Markdown/JSON

## Contributing

Contributions are welcome! If you'd like to improve the extension:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add my feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

Please make sure your changes pass the existing tests and add new tests for new functionality.

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

## Author

**Matteo Destriez — Terrighi** · [GitHub](https://github.com/Matteodt974)
