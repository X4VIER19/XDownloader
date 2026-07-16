# XDownloader
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

**XDownloader** es una aplicación de escritorio multiplataforma en desarrollo, orientada a la gestión y descarga de contenido multimedia. Construida con **C++ moderno**, el framework gráfico **Qt 6** y el sistema de construcción **CMake**, busca ofrecer una interfaz nativa, fluida y eficiente enfocada al público hispanohablante.

---

## ✅ Funcionalidades Implementadas

- Análisis de URLs mediante `yt-dlp -J` de forma asíncrona (no bloquea la interfaz)
- Selección de resolución y pista de audio reales obtenidas del video analizado
- Descarga y visualización de la miniatura del video
- Modos de descarga: Completo, Video+Audio, Solo Video, Solo Audio
- Cola de descargas secuencial con progreso en tiempo real (velocidad, ETA, tamaño, porcentaje)
- Selección de carpeta de destino con ruta por defecto (`Videos/XDownloader`)
- Validación de rutas de destino antes de iniciar la descarga
- Soporte para múltiples contenedores: MKV, MP4, WEBM

---

## 🚧 En Desarrollo

- Cancelación de descargas con limpieza de archivos temporales (`.part`)
- Soporte para lotes y playlists
- Descarga y muxing de subtítulos
- Pantalla de ajustes con preferencias persistentes (`QSettings`)
- Expansión del sidebar con animación hover

---

## 📦 Dependencias

| Herramienta | Versión mínima | Notas |
|-------------|---------------|-------|
| [Qt](https://www.qt.io/download) | 6.5 | Módulos: Widgets, Network |
| [CMake](https://cmake.org/) | 3.19 | Sistema de construcción |
| [yt-dlp](https://github.com/yt-dlp/yt-dlp) | cualquiera | Debe estar en el PATH del sistema |
| [FFmpeg](https://ffmpeg.org/) | cualquiera | Requerido por yt-dlp para muxing |

---

## 🛠️ Compilación

```bash
git clone https://github.com/X4VIER19/XDownloader.git
cd XDownloader
cmake -B build
cmake --build build
```

> En Windows se recomienda usar Qt Creator con el kit MinGW 64-bit incluido en la instalación de Qt.

---

## 📁 Estructura del Repositorio

```text
XDownloader/
│
├── CMakeLists.txt          # Configuración principal de CMake
├── main.cpp                # Punto de entrada de la aplicación
├── mainwindow.cpp          # Lógica e interacción de la ventana principal
├── mainwindow.h            # Definición de la clase principal, señales y slots
├── mainwindow.ui           # Diseño visual en formato XML (Qt Designer)
├── downloadtaskwidget.cpp  # Widget de tarea individual en la cola de descargas
└── downloadtaskwidget.h    # Definición del widget de tarea
```

---

## 👥 Autores y Desarrollo

- **Sergio Xavier Fernandez** ([X4VIER19](https://github.com/X4VIER19)) — Desarrollador Principal****
