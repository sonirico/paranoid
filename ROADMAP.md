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
- [x] Estado de menú (Play / Options / Quit, Options aún "coming soon"); Escape abre menú de pausa (Resume / Main menu / Quit) sin perder la partida
- [x] Fullscreen (F11 o desde Options) con letterbox/stretch configurable en Options
- [x] Score + HUD: 10 puntos por vida del ladrillo (solo al matarlo); high score persistido en `~/.local/share/paranoid/highscore`
- [x] Transiciones: "ROUND N / READY" al empezar stage (solo "READY" al respawnear), "GAME OVER" 3 s y vuelta al menú

## Nivel 3 — Contenido y pulido

- [x] Diseñar stages 5–10: Invader, Checkerboard, Fortress, Chevrons, Cage, END
- [x] Efectos de las cápsulas B/M/N: B = break (salta de stage), M = megaball (atraviesa ladrillos 10 s), N = net (barrera en el suelo 10 s)
- [x] Juice: partículas de color al romper ladrillo, screen shake al perder vida y en game over
- [x] Animación de muerte de la pala (5 frames 32x16 de `base.png` y=224)
- [x] Pala expandida con sus frames reales del sheet (48x8, y=160)
- [x] Música: tema en el menú (`arkanoid.ogg`), stages alternan `stage.ogg`/`crystalhammer.ogg`, silencio en game over (jingle = `muerte.wav`)
- [ ] Soporte de gamepad
- [x] Ajustes de volumen (música y fx) en Options, persistidos en `settings`
- [x] Cheat de la tecla `A` solo en builds con `DEV_MODE` (`make dev`)
- [ ] Editor de mapas: guardar/cargar niveles (`CMapState` solo imprime a stdout)
