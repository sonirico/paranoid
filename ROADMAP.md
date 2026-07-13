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
- [x] Input de ratón para la pala (absoluto, click = Espacio) + teclado a 550 px/s
- [x] Sonido al perder la bola (`media/fx/muerte.wav`)
- [x] Sonido al coger cápsulas (X→`lifeup.wav`, C→`sticky.wav`, resto→`option.wav`; `points.wav` reservado para el score)
- [x] Pausa real: Escape alterna pausa con overlay "PAUSA"; el juego solo se cierra con la X de la ventana

## Nivel 2 — Estructura de juego

- [x] Render de texto: bitmap font 8x8 (`engine::BitmapFont` + `engine::Text`, `media/textures/font.png`)
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
