# Design: Fase 1 - Fundação & Módulo "Architect"

**Data:** 2026-04-08  
**Versão:** 1.0  
**Projeto:** Convoy - The Codex Art Station

---

## 1. Visão Geral do Sistema

O Convoy será uma aplicação desktop modular construída sobre um **Shell Core** que carrega módulos dinâmicos. A arquitetura segue o padrão Host/Plugin, onde o core gerencia ciclo de vida, logging, e comunicação entre módulos.

### Estrutura de Diretórios

```
convoy/
├── bin/                    # Executáveis e DLLs
├── data/                   # Shaders, fonts, configurações globais
├── projects/               # Arquivos .cvp dos projetos
├── src/
│   ├── convoy/             # Aplicação principal (entry point)
│   ├── core/               # Sistema de janelas, filesystem, undo/redo
│   ├── modules/            # Módulos carregáveis
│   │   └── mod_architect/  # Editor de pixel art
│   ├── shared/             # Tipos comuns, utilitários
│   └── exporters/         # Exportadores (PNG, JSON, .cvp)
└── build/                  # Output de compilação
```

---

## 2. Core Engine (Shell)

### Componentes Principais

| Componente | Responsabilidade | API Principal |
|------------|------------------|---------------|
| **WindowManager** | Cria/manejinha janelas nativas via GLFW | `create_window()`, `set_workspace()` |
| **WorkspaceSystem** | Contextos de ferramentas (F1/F2/F3) | `push_workspace()`, `get_active_workspace()` |
| **FileSystem** | IO binário para .cvp | `save_project()`, `load_project()` |
| **CommandManager** | Undo/Redo stack genérica | `execute()`, `undo()`, `redo()` |
| **Logger** | Logging multi-nível com rotação | `log_info()`, `log_error()` |

### Diagrama de Comunicação

```
┌─────────────────────────────────────────┐
│              Convoy Shell               │
│  ┌──────────┐ ┌──────────┐ ┌─────────┐  │
│  │ Window   │ │ Command  │ │ File    │  │
│  │ Manager  │ │ Manager  │ │ System  │  │
│  └────┬─────┘ └────┬─────┘ └────┬────┘  │
│       └────────────┼────────────┘       │
│                    ▼                    │
│         ┌─────────────────┐             │
│         │   Event Bus     │             │
│         └────────┬────────┘             │
└──────────────────┼───────────────────────┘
                   ▼
        ┌─────────────────────┐
        │    Modules (DLLs)   │
        │  ┌───────────────┐  │
        │  │ mod_architect │  │
        │  └───────────────┘  │
        └─────────────────────┘
```

---

## 3. Módulo Architect (Pixel Editor)

### 3.1 Canvas System

| Feature | Descrição |
|---------|-----------|
| **Viewport** | Zoom 1x-32x com grid de pixels visível |
| **Multiple Layers** | até 32 layers com alpha blending |
| **Non-destructive** | Edits são bufferizados antes de aplicar |
| **Selection Tool** | Seleção retangular e laço (lasso) |

### Estruturas de Dados

```cpp
struct Canvas {
    uint32_t width, height;
    std::vector<Layer> layers;
    ActiveTool active_tool;
    Viewport viewport;
};

struct Layer {
    std::string name;
    bool visible;
    bool locked;
    float opacity;
    std::vector<uint32_t> pixels;
};
```

### 3.2 Ferramentas Essenciais

| Tool | Atalho | Comportamento |
|------|--------|----------------|
| **Pencil** | P | Desenha pixel individual |
| **Eraser** | E | Define alpha = 0 |
| **Bucket** | G | Flood fill com tolerance |
| **Color Picker** | I | Colore sample do canvas |
| **Move** | M | Move seleção ou layer |

### Arquitetura de Tools (Strategy Pattern)

```cpp
struct Tool {
    virtual void on_mouse_down(Canvas*, Point pos) = 0;
    virtual void on_mouse_drag(Canvas*, Point from, Point to) = 0;
    virtual void on_mouse_up(Canvas*, Point pos) = 0;
};
```

### 3.3 Metadata Editor (Pivot + Hitbox)

| Metadata | Tipo | UI |
|----------|------|-----|
| **Pivot Point** | Point (offset relativo) | Arrastar cruz no canvas |
| **Hitbox** | AABB (x, y, w, h) | Retângulo visual editável |

```cpp
struct SpriteMetadata {
    Point pivot;
    AABB hitbox;
    std::string name;
};
```

---

## 4. Sistema de Workspace Dinâmico

| Workspace | Atalho | Contexto |
|-----------|--------|----------|
| **Art** | F1 | Ferramentas de desenho ativadas |
| **World** | F2 | grid 16x16/32x32, tile picker |
| **Anim** | F3 | Timeline, frame control |

---

## 5. Formato .cvp (Binário - Fixed Structure)

```
┌──────────────────────────────────────────┐
│ CVP Header (16 bytes)                    │
│  - Magic: "CVPF" (4 bytes)              │
│  - Version: uint16                       │
│  - Layer Count: uint16                   │
│  - Canvas Width: uint32                  │
│  - Canvas Height: uint32                 │
├──────────────────────────────────────────┤
│ Metadata Section (variable)              │
│  - Pivot: int16 x 2                      │
│  - Hitbox: int16 x 4                     │
├──────────────────────────────────────────┤
│ Layer Data (per layer)                   │
│  - Name Length: uint8                    │
│  - Name: char[64]                        │
│  - Flags: uint8 (visible/locked)        │
│  - Opacity: float                        │
│  - Pixel Data: uint8[width*height*4]      │
└──────────────────────────────────────────┘
```

---

## 6. Stack Tecnológico

| Camada | Tecnologia | Justificativa |
|--------|------------|----------------|
| **Janelas** | GLFW 3.4 | Multi-plataforma, leve |
| **UI** | Dear ImGui + ImPlot | Rápido, estilo "Dark Terminal" |
| **Imagens** | stb_image + stb_image_write | Header-only, sem dependências |
| **Math** | Custom | Leve, cross-platform |
| **Build** | CMake | Multi-plataforma |

---

## 7. Critérios de Validação (Definition of Done)

- [ ] Aplicação compila em Windows e Linux
- [ ] Janela principal abre com ImGui renderizando
- [ ] Canvas exibe grid de pixels com zoom funcional
- [ ] 5 ferramentas básicas funcionam (Pencil, Eraser, Bucket, Picker, Move)
- [ ] Layers podem ser adicionadas/ocultadas/reescolhidas
- [ ] Pivot point pode ser definido via UI
- [ ] Hitbox pode ser definida e visualizada
- [ ] Save/Load em formato binário .cvp funciona
- [ ] Undo/Redo funciona para operações básicas
- [ ] Workspaces trocam via F1/F2/F3

---

## 8. Decisões de Design

| Decisão | Opção Escolhida | Justificativa |
|---------|-----------------|----------------|
| Formato .cvp | Binário (Fixed Structure) | Rápido para carregar, menor tamanho |
| Plataforma | Multi-plataforma (Win + Linux) | Requisito do utilizador |
| Prioridade | Arquitetura primeiro | Base sólida para expansão futura |
| Nível C++ | Avançado | Suporte a abstrações e padrões complexos |
| Fase 1 escopo | Tudo (Architect + Canvas + Layers + Workspace) | Requisito do utilizador |

---

*Documento aprovado pelo utilizador em 2026-04-08*
