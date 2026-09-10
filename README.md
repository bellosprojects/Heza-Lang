# Repositorio Oficial de Heza Lang

Heza es un lenguaje de programación interpretado que entiende y procesa notación matemática nativa.

### Tecnologias
- **Interpretado en Python:** Ejecuta un proceso de Lexer/Parser completo en Python para construir un AST (Abstract Sintax Tree) que será leído por el interprete.
- **Acelerado en C++:** Heza almacena y procesa todos los datos usando HSCE (Heza Symbolic Calculation Engine), un CAS (Computation Algebraic System) hecho en C++ capaz de representar, simplificar y calcular expresiones algebraicas, trigonométricas, de conjuntos y lógicas de manera eficiente empleando reglas de simplificación algebraica y normalización canonica. 
- **Punteros Compartidos:** Usa la librería de pybind11 para compartir referencias de memoria entre Python y C++ de manera eficiente.
- **Empaquetado para Windows**: Emplea InnoSetup para generar un instalador para Windows que asocia los archivos y registra `heza` en las variables de entorno para facilitar su uso.
- **UTF-8:** El Lexer reconoce tantos las palabras clave en ingles (ej. `union` `and`) como los simbolos matematicos nativos (ej. `∈` `∀`).
- **Modo Calculadora:** Usando la bandera `-calculator` Heza se comporta como una consola que recibe expresiones por la entrada estándar, resuelve el calculo y lo devuelve por la salida estándar, útil para realizar cálculos simples o complejos de manera rápida sin abrir un editor de código.

### Extensión de VS Code
Para facilitar la escritura de los símbolos matemáticos UNICODE Heza tiene una extensión publicada en el marketplace de VS Code que cuenta con las siguientes caracteristicas:
- **Snippets de Símbolos:** Ofrece autocompletado para los símbolos matemáticos a través de palabras clave.
- **Resaltado Semántico:** Resalta las palabras claves, variables y funciones declaradas por el usuario.
- **Go to definition:** Usa Ctrl + click o F12 para ir a la definición de una función o variable.
- **Hover documentación:** Analiza los comentarios anteriores a una variable y función y las renderiza como la documentación del objeto.
- **Signature Help:** Muestra ayuda para completar los parámetros de una funcion.
- **Detección de Errores:** Detecta errores lexicos, de sintaxis y en tiempo de ejecucion (como el uso de variables no declaradas o usadas antes de la declaracion).
- **Linter:** Detecta variables no utilizadas y resalta la advertencia.
- **Ayuda Contextual:** Todas las funciones del LSP se analizan mediante contextos, por lo cual una variable declarada dentro de una función no se detecta como variable fuera de esa funcion.
- **Inferencia de Tipos:** A pesar de no ser un lenguaje tipado, el LSP aplica una función para inferir el tipo de dato de una variable y ofrecer los métodos disponibles.
- **Comando de Ejecución:** Agrega un botón con el icono play que registra dispara el comando `heza.runScript` y ejecuta el archivo `.hz`.

### Enlaces

[Descarga el instalador oficial para Windows x64 desde este repositorio](https://github.com/bellosprojects/Heza-Lang/releases/download/v1.0.0/HezaSetup-Windows.exe)

[Instala la Extensión Oficial para VS Code](https://marketplace.visualstudio.com/items?itemName=BellosProjects.heza-support)

Visita la [Web Oficial de Heza Lang]() para tener acceso a una guía de instalación, ejemplos de código, documentación completa y ayuda a preguntas frecuentes. 
