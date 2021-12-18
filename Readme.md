# SO_TP2
Desarrollo de un Sistema Operativo 64 bits basado en el proyecto x64BareBones de Rodrigo Rearden (RowDaBoat).
<p>El sistema operativo cuenta con un kernel monolítico de 64 bits, manejo de interrupciones,
system calls, driver de teclado, driver de video y binarios de kernel space y user space separados.
</p>
<p>Además se cuenta con dos administradores de memoria. Tanto el kernel como los procesos pueden utilizar estos administradores de memoria
física. Es importante aclarar que no funcionan al mismo tiempo, sino que se decide cuál utilizar en tiempo de compilación.
</p>

<h4> Memory Manager </h4>
<ul>
  <li>Reservar y liberar memoria</li>
  <li>Consultar el estado de la memoria</li>
</ul>

<h4> Scheduler </h4>
<p>El sistema también cuenta con multitasking pre-emptivo con una cantidad variable
de procesos. Para ello el sistema implementa un sistema que permite suspender la ejecución de un proceso y continuar la ejecución de otro (context switching)
con el algoritmo Round Robin modificado para contemplar prioridades.
(scheduler).
</p>
<ul>
  <li>Crear procesos (con argumentos)</li>
  <li>Obtener el PID del proceso que llama</li>
  <li>Listar todos los procesos: nombre, ID, prioridad, stack y base pointer y foreground</li>
  <li>Matar un proceso arbitrario</li>
  <li>Modificar la prioridad de un proceso arbitrario.</li>
  <li>Bloquear y desbloquear un proceso arbitrario</li>
</ul>

<h4> Semaphore Manager </h4>
<p>El sistema implementa semáforos y un mecanismo para que procesos no
relacionados puedan compartirlos acordando un identificador a priori. El sistema también
permite listar todos los semáforos activos del sistema, su estado y los procesos
bloqueados en cada uno. Está libre de busy waiting, deadlock, race conditions y además las instrucciones utilizadas garantizan atomicidad.</p>
<ul>
  <li>Crear, abrir y cerrar un semáforo.</li>
  <li>Modificar el valor de un semáforo.</li>
  <li>Listar el estado de todos los semáforos del sistema</li>
</ul>

<h4> Interprocess Communication </h4>
<p>El sistema implementa pipes unidireccionales y las operaciones de lectura
y escritura sobre los mismos son bloqueantes. Todo proceso es capaz
de escribir/leer tanto de un pipe como de la pantalla sin necesidad de que su código sea
modificado. Notar que esto permite que el intérprete de comandos conecte 2 programas
utilizando un pipe y a su vez, estos programas podrán ser ejecutados de
forma aislada. El sistema también permite que procesos no relacionados puedan
compartirlos acordando un identificador a priori.
</p>
<ul>
  <li>Crear y abrir pipes.</li>
  <li>Leer y escribir de un pipe.</li>
  <li>Listar el estado de todos los pipes del sistema.</li>
</ul>

<h4> Aplicaciones de User Space </h4>

<ul>
  <li>help.</li>
  <li>wc.</li>
  <li>cat.</li>
  <li>filter.</li>
  <li>ps.</li>
  <li>nice.</li>
  <li>block.</li>
  <li>kill.</li>
  <li>phylo.</li>
  <li>otros (consultar help).</li>
</ul>

<h2>Autores</h2>
<ul>
  <li><a href="https://github.com/ssandrini">Santiago Sandrini</a></li>
  <li><a href="https://github.com/solkonfe">Sol Konfederak</a></li>
  <li><a href="https://github.com/florchao">Florencia Chao</a></li>
</ul>
