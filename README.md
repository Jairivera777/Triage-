# Triage-

Hola!! Agradecemos de antemano que quieran ver nuestro proyecto!!

Nuestro proyecto se basa en un Triage hospitalario, en donde diligencias tus datos y el sistema te genera un triage. En base a este, seras atendido antes o despues. Esto se hace con ayuda de html, javaScript y c++ que son los lenguajes usados en este proyecto. 

Tambien tiene un filtro en donde puedes consultar los pacientes que estan en fila y los puedes filtrar por triage o puedes mostrar todos los pacientes. 

Por ultimo en el panel consultas puedes ver el paciente que tienes que antender, ver sus datos y despues de eso diagnosticarlo y decir que se hizo para en un futuro agregarlo en la historia clinica. El proyecto sigue en desarrollo y espero les guste d: Como tal creemos que hubo en 50% de codigo generado por IA.

PARA PODERLO EJECUTAR:

    Tienes que tener c++ descargado en tu computadora.
    debes colocar en la terminal: "g++ main.cpp -o servidor.exe -lws2_32". Presiona la tecla enter.
    Para terminar, ahora coloca "./servidor.exe". Presiona la tecla enter y el server en c++ ya estara corriendo.

    *Si te pide permisos de fireWall, dile que si*

    Abre la interfaz.html y agrega datos. Ya con lo que hicimos utilizara a c++ como backend y estara conectado con la interfaz

Estructuras de datos utilizadas:

*COLAS CON PRIORIDAD:* Se utilizaron para poder hacer el triage, debido a que cada triage tiene una prioridad distinta (el triage 1 se atiende primero, luego el 2, el 3, el 4 y por ultimo el 5). Esto nos permite optimizar de manera muy facil y con menos complejidad los pacientes, todos estos siguiendo un orden.

*ARREGLOS DINAMICOS:* Los arreglos dinamicos se usaron para poder implementar las colas con prioridad. Estos arreglos van creciendo o decreciendo en base a la cantidad de pacientes que hay. Como tal la cola con prioridad se encarga de organizar este arreglo de tal manera que entre menor nivel de Triage tenga, va a estar mucho mas cerca del primer paciente que es el siguiente en ser atendido.

*PILAS:* Se utilizaron para llevar el registro en donde logramos ver el historial de los pacientes atendidos. Tambien en caso de que algun medico atienda a un paciente y termine la consulta con error, pueda volver a entrar en la fila y como conserva sus datos entraria en primer lugar, para que asi el doctor lo termine de atender.

Video de la explicacion: https://drive.google.com/file/d/1n5tRtFyYfwISSGVO1AOK5GSxBt7eewju/view?usp=drive_link