# 🧱 Breakout C (Terminal / Mobile)

Uma implementação clássica do jogo **Breakout** (estilo *Arkanoid*) desenvolvida inteiramente em linguagem **C**, projetada para rodar diretamente no terminal com interface colorida e controles responsivos.

O layout da tela foi dimensionado especificamente para oferecer uma boa experiência tanto em computadores quanto em IDEs e terminais de **dispositivos móveis** (em modo retrato).

---

## ✨ Funcionalidades

- **Renderização no Terminal:** Gráficos em ASCII com suporte a cores via sequências de escape ANSI.
- **Modo Não-Bloqueante:** Leitura instantânea de teclas sem necessidade de pressionar `Enter` (utilizando `termios` e `fcntl`).
- **Física de Rebote Dinâmica:** O ângulo de retorno da bola muda dependendo do ponto de impacto na raquete.
- **Sistema de Múltiplas Bolas (Power-Up):** Ao atingir metas de pontuação ou coletar itens especiais, a bola se multiplica para acelerar a destruição dos blocos.
- **Progressão de Fases:** Aumento de fases e recompensa de vidas extras ao limpar todos os blocos da tela.

---

## 🎮 Controles

| Tecla | Ação |
| :---: | :--- |
| **`A`** | Move a raquete para a esquerda |
| **`D`** | Move a raquete para a direita |
| **`P`** | Pausa / Continua o jogo |
| **`Q`** ou **`S`** | Encerra o jogo |

---

## 🛠️ Como Compilar e Executar

### Requisitos
- Compilador C (`gcc` ou `clang`)
- Sistema operacional compatível com POSIX (Linux, macOS, Termux no Android, WSL no Windows)

### Passos

1. **Clone o repositório:**
   ```bash
   git clone [https://github.com/seu-usuario/nome-do-repositorio.git](https://github.com/seu-usuario/nome-do-repositorio.git)
   cd nome-do-repositorio
# 🧱 Breakout C (Terminal / Mobile)

Uma implementação clássica do jogo **Breakout** (estilo *Arkanoid*) desenvolvida inteiramente em linguagem **C**, projetada para rodar diretamente no terminal com interface colorida e controles responsivos.

O layout da tela foi dimensionado especificamente para oferecer uma boa experiência tanto em computadores quanto em IDEs e terminais de **dispositivos móveis** (em modo retrato).

---

## ✨ Funcionalidades

- **Renderização no Terminal:** Gráficos em ASCII com suporte a cores via sequências de escape ANSI.
- **Modo Não-Bloqueante:** Leitura instantânea de teclas sem necessidade de pressionar `Enter` (utilizando `termios` e `fcntl`).
- **Física de Rebote Dinâmica:** O ângulo de retorno da bola muda dependendo do ponto de impacto na raquete.
- **Sistema de Múltiplas Bolas (Power-Up):** Ao atingir metas de pontuação ou coletar itens especiais, a bola se multiplica para acelerar a destruição dos blocos.
- **Progressão de Fases:** Aumento de fases e recompensa de vidas extras ao limpar todos os blocos da tela.

---

## 🎮 Controles

| Tecla | Ação |
| :---: | :--- |
| **`A`** | Move a raquete para a esquerda |
| **`D`** | Move a raquete para a direita |
| **`P`** | Pausa / Continua o jogo |
| **`Q`** ou **`S`** | Encerra o jogo |

---

## 🛠️ Como Compilar e Executar

### Requisitos
- Compilador C (`gcc` ou `clang`)
- Sistema operacional compatível com POSIX (Linux, macOS, Termux no Android, WSL no Windows)

### Passos

1. **Clone o repositório:**
   ```bash
   git clone [https://github.com/seu-usuario/nome-do-repositorio.git](https://github.com/seu-usuario/nome-do-repositorio.git)
   cd nome-do-repositorio
