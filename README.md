# Triage-

Hola!! Agradecemos de antemano que quieran ver nuestro proyecto!!

Nuestro proyecto se basa en un Triage hospitalario, en donde diligencias tus datos y el sistema te genera un triage. En base a este, seras atendido antes o despues. Esto se hace con ayuda de html, javaScript y c++ que son los lenguajes usados en este proyecto. 

Tambien tiene un filtro en donde puedes consultar los pacientes que estan en fila y los puedes filtrar por triage o puedes mostrar todos los pacientes. 

Por ultimo en el panel consultas puedes ver el paciente que tienes que antender, ver sus datos y despues de eso diagnosticarlo y decir que se hizo para en un futuro agregarlo en la historia clinica. El proyecto sigue en desarrollo y espero les guste d: Como tal creemos que hubo en 50% de codigo generado por IA.

Estructuras de datos utilizadas:

*COLAS CON PRIORIDAD:* Se utilizaron para poder hacer el triage, debido a que cada triage tiene una prioridad distinta (el triage 1 se atiende primero, luego el 2, el 3, el 4 y por ultimo el 5). Esto nos permite optimizar de manera muy facil y con menos complejidad los pacientes, todos estos siguiendo un orden.

*ARREGLOS DINAMICOS:* Los arreglos dinamicos se usaron para poder implementar las colas con prioridad. Estos arreglos van creciendo o decreciendo en base a la cantidad de pacientes que hay. Como tal la cola con prioridad se encarga de organizar este arreglo de tal manera que entre menor nivel de Triage tenga, va a estar mucho mas cerca del primer paciente que es el siguiente en ser atendido.

*LISTAS SIMPLES:* Se utilizaron a la hora de consultar a los pacientes para poderlos filtrar. Como tal se puede hacer con el arreglo dinamico, pero preferimos utilizar las listas debido a que si utilizaramos el arreglo dinamico, y en este como se guardan los pacientes de manera organizada. En caso de cualquier error estos datos se pueden perder. Entonces copiamos estos elementos en una lista y con esto nos aseguramos de que el arreglo dinamico no quede muy sobrecargado y guarde los elementos. Luego esta lista la organizamos en un json y la mandamos a la interfaz.