# XDownloader

![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Qt](https://img.shields.io/badge/Qt-41CD52?style=for-the-badge&logo=qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

**XDownloader** es una aplicación de escritorio multiplataforma en desarrollo orientada a la gestión y descarga de contenido multimedia. El proyecto está siendo construido utilizando **C++**, el framework gráfico **Qt 6** y el sistema de construcción **CMake**, buscando ofrecer una interfaz nativa, fluida y eficiente.

---

## 🛠️ Estado y Mecánica del Proyecto

Al encontrarse actualmente en fase activa de desarrollo, el software se enfoca en la estructuración de su arquitectura base:

1. **Interfaz Gráfica (UI):** Diseño modular basado en componentes nativos de Qt (`mainwindow.ui`) para la gestión de enlaces y control de descargas.
2. **Lógica de Control:** Arquitectura orientada a objetos en C++ para preparar la futura integración con herramientas de extracción multimedia y manejo de flujos de red.
3. **Construcción Portable:** Configuración automatizada mediante CMake para facilitar su compilación en diferentes sistemas operativos de manera unificada.

---

## 📁 Estructura del Repositorio

La distribución del código fuente y los recursos de la interfaz se organiza de la siguiente manera:

```text
XDownloader/
│
├── CMakeLists.txt      # Archivo de configuración principal de CMake para compilar con Qt
├── main.cpp            # Punto de entrada de la aplicación (inicializa el ciclo de vida de Qt)
├── mainwindow.cpp      # Implementación de la lógica e interacción de la ventana principal
├── mainwindow.h        # Definición de la clase principal, señales y slots de la interfaz
└── mainwindow.ui       # Diseño visual de la interfaz gráfica en formato XML (Qt Designer)
```

---

👥 Autores y Desarrollo
    Sergio Xavier Fernandez (X4VIER19) - Desarrollador Principal
