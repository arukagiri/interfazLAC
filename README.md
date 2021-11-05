# LACAN GUI

Projecto final para el laboratorio de control (LAC) de la facultad de ciencias exactas, ingeniería y agrimensura (FCEIyA).

El mismo consiste en una interfaz de usuario gráfica (GUI) que permite la conexión de una PC a la red de comunicaciones CAN del laboratorio (LACAN).

Esta red CAN interconecta a su vez la red de energía electrica del laboratorio, permitiendo consultar y comandar los dispositivos que la componen para su fácil administración.

## Clonación del repositorio y uso
Principalmente este repositorio esta orientado a dar material de referencia a futuros Ingenieros de la facultad que necesiten realizar una GUI para sus proyectos.

El proyecto debería de funcionar sin mucho problema, salvo la verificación de ciertos requisitos. Los archivos de *source*, *header* y *GUI* se encuentran bajo el directorio fuente, pero esto no es problema debido a que Qt Creator se encarga de dividirlos en carpetas para su facil manejo.

### Prerrequisitos

Debido a la integridad de este proyecto fue desarrollado con el framework para C++ orientado a GUIs llamado Qt, es **fundamental** su instalación previa.

En la misma debe incluirse el compilador para C++ MinGW.

## Deployment

Para realizar el *deployment* de la aplicación en Windows debe ejecutarse el aplicativo nativo del framwork llamado **windeployqt.exe** (encontrado generalmente en la carpeta de instalación del programa, por defecto *C:\Qt\5.11.2\mingw53_32\bin*).

### Pasos a seguir
* Realizar una compilación del tipo *release* con Qt creator.
* Entrar a la carpeta generada y navegar hasta el directorio interno también llamado *release* (Ej. C:\Qt\workspace\build-test3-Desktop_Qt_5_11_2_MinGW_32bit-Release\release).
* Borrar todos los archivos dentro de *release* menos el **.exe** de la aplicación.
* Ejecutar **windeployqt.exe** usando como parametro el *path* de dicho **.exe**. 

**NOTA:** El directorio de release debe tener incluido el path del compilador y las herramientas de Qt (Ej. C:\Qt\Tools\mingw530_32\bin).

## Construido con

* [Qt framework](https://www.qt.io/) - GUI Framework

## Autores

* **Lucas Sandoz** - *Trabajo inicial* - [AraragiRukasu](https://github.com/AraragiRukasu)
* **Agustín Baffo** - *Trabajo inicial* - [AgustinBaffo](https://github.com/AgustinBaffo)

Ver lista de [contribuidores](https://github.com/your/project/contributors) que participaron en el proyecto.

## Licencia

Este proyecto está licenciado con BSD-3-Clause License - ver el archivo [LICENSE.md](LICENSE.md) para más detalles.

