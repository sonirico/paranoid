# Stage files

Each `.txt` file here is one stage, played in filename order: 15 lines
of 15 characters, one per brick cell, top to bottom. Lines starting
with `#` are comments and blank lines are ignored.

| Char | Brick                  |
|------|------------------------|
| `.`  | empty                  |
| `R`  | red (1 life)           |
| `O`  | orange (1)             |
| `Y`  | yellow (1)             |
| `G`  | green (1)              |
| `A`  | aqua (1)               |
| `B`  | blue (1)               |
| `P`  | purple (1)             |
| `W`  | white (1)              |
| `S`  | silver (2 lives)       |
| `D`  | gold (3 lives)         |
| `X`  | indestructible         |

A brick scores 10 points per life when destroyed. A stage needs at
least one destructible brick or it is skipped.

The in-game editor (EDITOR in the main menu) saves this same format to
`~/.local/share/paranoid/custom.map`; copy that file here with the next
free number to ship it as a built-in stage.
