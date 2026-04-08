# 🗺️ Convoy Development Roadmap

## Fase 1: Fundação & Módulo "Architect" (O Nascimento)
**Objetivo:** Substituir a necessidade de editores externos para sprites simples e estabelecer o sistema de arquivos.

* **Março/Maio 2026:**
    * [ ] **Core Engine:** Implementação do shell básico (janelas, docking, menus) usando Dear ImGui.
    * [ ] **Architect V1:** Ferramentas essenciais de desenho (Pencil, Eraser, Bucket, Color Picker).
    * [ ] **Canvas System:** Suporte a múltiplas camadas (Layers) e transparência (Alpha).
    * [ ] **Metadata Editor:** Sistema para definir o **Pivot Point** (âncora) e a primeira **Hitbox** (AABB) diretamente no sprite.
    * [ ] **IO Inicial:** Salvar e carregar no formato proprietário `.cvp` (Codex Visual Project - JSON por enquanto).

## Fase 2: Módulo "Forge" & "Sequencer" (Movimento e Otimização)
**Objetivo:** Transformar imagens isoladas em animações e otimizar para a GPU.

* **Junho/Agosto 2026:**
    * [ ] **Forge (Atlas Packer):** Algoritmo de bin-packing para unir múltiplos sprites em um único Texture Atlas.
    * [ ] **Sequencer (Timeline):** Criação de animações frame-a-frame com controle de FPS.
    * [ ] **State Machine:** Interface visual para definir transições (ex: "Idle" para "Run" quando a velocidade > 0).
    * [ ] **DOD Profiler:** Visualizador de memória que mostra quantos KB aquele Atlas ocupará na VRAM.

## Fase 3: Módulo "Walker" (Construção de Mundo)
**Objetivo:** Permitir a montagem de cenários e definição de dados de colisão complexos.

* **Setembro/Novembro 2026:**
    * [ ] **Walker Grid:** Sistema de Tilemap com suporte a diferentes tamanhos de grid (16x16, 32x32).
    * [ ] **Z-Depth Layers:** Organização de camadas de cenário (Background, Midground, Foreground).
    * [ ] **Collision Painting:** Ferramenta para pintar propriedades de "parede", "escada" ou "água" sobre o mapa.
    * [ ] **Entity Spawner:** Colocar "placeholders" de inimigos ou itens que serão lidos pelo ECS da Caffeine.

## Fase 4: Integração Profunda & Exportação .CAF (O Porto)
**Objetivo:** Conectar o Convoy diretamente ao binário da Engine.

* **Dezembro/Fevereiro 2027:**
    * [ ] **Binary Compiler:** Implementação do exportador `.caf` (tipo Texture e tipo Scene).
    * [ ] **32-byte Alignment:** Garantir que o output binário siga a especificação de memória da Fase 1 da Engine.
    * [ ] **Hot-Reload Bridge:** Plugin que detecta quando o Convoy salvou um asset e avisa a Engine para recarregar instantaneamente.
    * [ ] **Live Preview:** Filtros de pós-processamento (CRT/Scanline) dentro do Convoy para testar a fidelidade visual.

---

## 🚦 Critérios de Sucesso por Fase

* **Fase 1:** Conseguir desenhar um personagem e definir onde ele "pisa" (Pivot) sem usar o Aseprite.
* **Fase 2:** Gerar um Spritesheet único com arquivo de dados de animação lido com sucesso por um protótipo.
* **Fase 3:** Montar uma fase de teste completa com colisões que a engine reconheça automaticamente.
* **Fase 4:** O tempo entre "Salvar no Convoy" e "Ver no Jogo" ser inferior a 2 segundos.

---

## 🛠️ Ferramentas Recomendadas para o Desenvolvimento
* **Linguagem:** C++ (para compartilhar a lógica de memória/math da Caffeine).
* **UI:** Dear ImGui (Interface rápida e focada em ferramentas).
* **Imagens:** `stb_image.h` e `stb_image_write.h` (Leitura/Escrita de PNG).
* **Janelas:** GLFW ou a própria SDL3 (que você já usará na Engine).
