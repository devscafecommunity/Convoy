# 🚛 Project Convoy: The Codex Art Station

> **"Onde o pixel ganha inteligência."**

O **Convoy** é o hub de criação artística oficial e proprietário do **Codex Studio**. Diferente de editores de imagem genéricos, o Convoy é construído com "consciência de hardware", garantindo que cada asset criado esteja intrinsecamente ligado aos requisitos técnicos da **Caffeine Engine**.

---

## 🏗️ 1. Visão Geral
O Convoy não é apenas um editor de pixels; é uma **ferramenta de tradução de dados**. Ele elimina o abismo entre o design artístico e a implementação técnica (DOD). Nele, a arte deixa de ser um arquivo estático e passa a carregar metadados críticos: colisão, pontos de ancoragem (pivots), máquinas de estado e camadas de profundidade.

---

## 🛠️ 2. Módulos Integrados (As Vagões do Convoy)

O sistema é modular, permitindo que cada "vagão" foque em uma especialidade mantendo a consistência do projeto.

| Módulo | Função Primária | Valor para a Caffeine Engine |
| :--- | :--- | :--- |
| **Architect (Pixel)** | Desenho e edição de Pixel Art. | Define **Pivot Points** e **Hitboxes** nativamente no nível do pixel. |
| **Forge (Atlas)** | Empacotamento de Spritesheets. | Otimiza **Draw Calls** (DOD) através de algoritmos de bin-packing. |
| **Sequencer (Anim)** | Timeline e Máquinas de Estado. | Gera o bytecode de animação para o `AnimationSystem`. |
| **Walker (World)** | Edição de Mapas e Tiles. | Define camadas de **Z-Depth** e mapas de colisão binários. |

---

## 📈 3. Roadmap de Desenvolvimento

### **Fase 1: O Workspace (Fundação)**
* **Interface Unificada:** Sistema de abas/janelas flutuantes baseado em *workspaces* contextuais.
* **Codex Project Format:** Um sistema de arquivos unificado (`.cvp`) que encapsula todos os sub-assets do projeto.
* **Visualizador de Alta Precisão:** Zoom infinito com grid de pixels e suporte a camadas (layers) não destrutivas.

### **Fase 2: Fluxo de Assets (Simbiose)**
* **Cross-Module Link:** Alterar um pixel no *Architect* atualiza instantaneamente o tile correspondente no *Walker*.
* **Data-Painting:** Ferramenta de pincel para "pintar" propriedades físicas (ex: áreas de dano, fricção, spawners) diretamente sobre o cenário.
* **Hot-Update Interno:** Sistema de atualização em tempo real para visualização de mudanças em layouts de mapas.

### **Fase 3: O Porto de Exportação (.CAF)**
* **Deploy to Caffeine:** Compilação de assets para o formato proprietário `.caf`.
* **Hardware Alignment:** Conversão automática para texturas comprimidas com alinhamento de memória de 32 bytes (otimizado para GPU).
* **Prefab Generation:** Exportação de entidades completas (Sprites + Colliders + Scripts de Animação) prontas para o ECS.

---

## 📐 4. Arquitetura do Sistema (Host/Plugin)

O Convoy é construído como um **Shell** de alta performance que carrega módulos dinâmicos, permitindo expansões futuras sem reconstruir o core.

```text
Convoy/
├── core/               # Gerenciamento de janelas, IO de arquivos, Undo/Redo
├── modules/            # Módulos de especialidade (Plugins Internos)
│   ├── mod_architect/  # Canvas, ferramentas de rasterização, pivots
│   ├── mod_walker/     # Grids, tile picking, layers de colisão
│   └── mod_forge/      # Atlas packing, mipmap generation
├── shared/             # Definições de tipos comuns e buffers de memória
└── exporters/          # Plugins de saída (PNG, JSON e o binário .CAF)
```

---

## 🎨 5. Interface & UX (Codex Style)

A estética segue o padrão **Dark Terminal** do Codex Studio, priorizando contraste e foco técnico.

* **Workspaces Dinâmicos:** Atalhos `F1` (Art), `F2` (World), `F3` (Anim) para troca instantânea de contexto de ferramentas.
* **Live Preview Window:** Janela flutuante que renderiza o asset na resolução nativa do jogo (ex: 320x180), incluindo simuladores de:
    * *Scanlines* e filtros CRT.
    * Paleta de cores limitada (estilo console clássico).
* **DOD Visualizer:** Modo de visualização de depuração que destaca em cores diferentes os dados estáticos (cenário) e dinâmicos (entidades), permitindo ao artista ver a carga de processamento do mapa.

---

## 💡 6. Por que o Convoy é o "Killer App" do Codex?

1.  **Independência Criativa:** Elimina a dependência de ferramentas externas (Aseprite, Tiled, TexturePacker), reduzindo custos e fragmentação de pipeline.
2.  **Contexto de Colisão Real:** Diferente de outras ferramentas, no Convoy você não "chuta" a colisão; você a desenha como parte integrante da estrutura do sprite ou do tile.
3.  **Prototipagem "Pre-Engine":** Os *Artisans* podem testar a navegação e o *feel* das animações dentro do próprio Convoy usando uma simulação física simplificada.
4.  **Zero-Overhead de Importação:** Ao exportar em `.caf`, o asset está pronto para ser mapeado na memória da **Caffeine Engine**, eliminando o tempo de processamento/parsing durante o carregamento do jogo.

---

> **Status do Projeto:** 📅 *Planejado para iniciar pós-Fase 1 da Engine.*
> **Responsável:** *Artisans & Tooling Architects.*
