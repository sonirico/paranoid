# Roadmap

Lista de trabajo para llevar Paranoid de "funciona" a "juego top". Tres niveles
por prioridad; dentro de cada nivel el orden es orientativo.

## Nivel 1 — Game feel

- [x] Velocidad de la bola normalizada (módulo constante) + rampa de aceleración por rebote
- [x] Spin + efecto Magnus como bonus (cápsula T), con tope de spin y banda de ±20° sobre la horizontal
- [x] Interpolación de render sobre el timestep fijo (fluidez en monitores >60 Hz)
- [x] Multibola estilo Arkanoid: la cápsula D divide en 3 bolas; drops al 10%
- [x] Fix sticky (cápsula C): la bola atrapada guarda su velocidad de despegue y se suelta con Espacio
- [x] Umbral de pala: la bola pasada del borde superior ya no es colisionable (sin rebote lateral)
- [x] Transformación de la pala en modo láser (frames de cañones azules del sheet)
- [x] HUD del bonus activo: icono de cápsula + nombre (texto debug 8x8 de SDL vía `Window::drawText`)
- [x] Timeout de bonus (10 s, `BONUS_DURATION`) con countdown en el HUD; cualquier cápsula cancela el modo anterior
- [x] Sticky atrapa la bola donde impactó (offset sobre la pala) en vez de recentrarla
- [ ] Input de ratón para la pala (+ revisar velocidad de teclado)
- [ ] Sonido al perder la bola (`media/fx/muerte.wav`, ya existe sin usar)
- [ ] Sonido al coger cápsulas (`option.wav`, `points.wav`, `lifeup.wav`, `sticky.wav`, sin usar)
- [ ] Pausa real: Escape no debe cerrar el juego en seco

## Nivel 2 — Estructura de juego

- [ ] Render de texto (bitmap font o SDL3_ttf) — prerequisito de menú, score y HUD
- [ ] Estado de menú (Play / Options / Quit); Escape vuelve al menú
- [ ] Fullscreen (F11) con modo letterbox (respeta aspecto) / stretch configurable
- [ ] Score + HUD (puntos por ladrillo/bonus) y high score persistido
- [ ] Transiciones: "Round N — Ready", pantalla de game over, stage clear

## Nivel 3 — Contenido y pulido

- [ ] Diseñar stages 5–10 (hoy vacíos en `assets.h`)
- [ ] Efectos de las cápsulas B/M/N (B = break/saltar nivel, etc.)
- [ ] Juice: partículas al romper ladrillo, flash/shake
- [ ] Animación de muerte de la pala (frames de explosión en `base.png` y=224–232)
- [ ] Pala expandida con sus frames reales del sheet (hoy se estira por escala; hay sprites anchos en y=176–184)
- [ ] Música por stage (`stage.ogg`, `crystalhammer.ogg` sin usar) + jingle de muerte
- [ ] Soporte de gamepad
- [ ] Ajustes de volumen (música y fx)
- [ ] Quitar el cheat de la tecla `A` (spawn de bola)
- [ ] Editor de mapas: guardar/cargar niveles (`CMapState` solo imprime a stdout)
