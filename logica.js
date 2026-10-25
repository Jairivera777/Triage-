document.getElementById('formTriage').addEventListener('keydown', function(e) {
    if (e.key === 'Enter') {
        e.preventDefault(); 
        return false;
    }
});
function actualizarHora(){ // funcion encargada de actualizar la hora de la interfaz cada segundo
    const ahora = new Date();
    let horas = ahora.getHours().toString().padStart(2, '0');
    let minutos = ahora.getMinutes().toString().padStart(2, '0');
    let segundos = ahora.getSeconds().toString().padStart(2, '0');
    document.getElementById('Hora').textContent = `${horas}:${minutos}:${segundos}`;
}
actualizarHora();
    setInterval(actualizarHora, 1000);
    function irA(modulo){ // funcion que se encarga de sobreescribir la pagina segun el click que de el usuario
    document.querySelectorAll('.seccion-panel').forEach(pantalla => {
       pantalla.classList.remove('activa');
    });
    document.querySelectorAll('.pestana').forEach(tab => {
        tab.classList.remove('activa');
    });
    document.getElementById('pantalla-'+modulo).classList.add('activa');
    document.getElementById('tab-'+modulo).classList.add('activa');
    if(modulo==='triage') cargarPacientes();
    if(modulo==='consulta') cargarPaciente();
    if(modulo==='historial') cargarHistorial();
};
const selectParentesco = document.getElementById('parentesco');
const bloqueAntes = document.getElementById('antes-parentesco')
const bloqueDespues = document.getElementById('despues-parentesco');
selectParentesco.addEventListener('change', function() { //se encarga de que si no tiene parentesco no aparezcan casillas
    if(this.value === 'nadie'){
        bloqueAntes.style.display = 'none';
        bloqueDespues.style.display = 'none';
    }else{
        bloqueAntes.style.display = 'block';
        bloqueDespues.style.display = 'block';
    }
});
const selectSexo = document.getElementById('Sexo');
const seccionEmbarazo = document.getElementById('seccion-embarazo');
selectSexo.addEventListener('change', function(){ //Si no es mujer no aaprece la funcion de embarazo
    if(this.value === 'Femenino'){
        seccionEmbarazo.style.display = 'block';
    }else{
        seccionEmbarazo.style.display = 'none'
    }
});
const selectDocumento = document.getElementById('tipoDocumentoPaciente');
const antesDocumento = document.getElementById('antesDocumento');
const despuesDocumento = document.getElementById('despuesDocumento');
selectDocumento.addEventListener('change', function(){ //Si esta no identifiado no pide los demas datos
    if(this.value === 'AS' || this.value === 'MS' || this.value === 'SI'){
        antesDocumento.style.display = 'none';
        despuesDocumento.style.display = 'none';
    } else{
        antesDocumento.style.display = 'block';
        despuesDocumento.style.display = 'block';
    }
});
document.getElementById('formTriage').addEventListener('submit', function(e) { // se encarga de conectar el front con c++
    e.preventDefault();
    const formData = new FormData(document.getElementById('formTriage'));
    const params = new URLSearchParams(formData);
    fetch('http://localhost:8080/api/triage' , {
        method: 'POST',
        body: params
    })
        .then(response => response.json())
        .then(data => {
            if (data.status === "ok") {
                const notif = document.getElementById('notificacionExito');
                document.getElementById('nivelTriageExito').textContent = data.triage;
                notif.style.display = 'block';
                setTimeout(() => {
                    notif.style.display = 'none';
                }, 4000);
                document.getElementById('formTriage').reset();
                document.getElementById('seccion-embarazo').style.display = 'none';
                cargarPacientes();
            }
        })
        .catch(error => {
            console.error('Error al conectar con C++: ', error);
            alert("No se logro conectar con C++");
        });
});
let listaGlobalPacientes = [];
function cargarPacientes(){
    fetch('http://localhost:8080/api/pacientes')
        .then(response => response.json())
        .then(data=> {
            if(Array.isArray(data))listaGlobalPacientes = data;
            else listaGlobalPacientes = [];
            renderizarPacientes();
        })
        .catch(err => console.error("Error al cargar pacientes", err));
}
function renderizarPacientes(){
    const selector = document.getElementById('filtros');
    const filtroValor = selector ? selector.value : 'TODOS';
    const contenedorLista = document.getElementById('listaPacientes');
    const contenedorT1 = document.getElementById('totalesT1');
    const contenedorT2 = document.getElementById('totalesT2');
    const contenedorT3 = document.getElementById('totalesT3');
    const contenedorT4 = document.getElementById('totalesT4');
    const contenedorT5 = document.getElementById('totalesT5');
    const contenedorTotales = document.getElementById('pacientesTotales');
    if(!contenedorLista) return;
    contenedorLista.innerHTML = '';
    const pacientesFiltrados = listaGlobalPacientes.filter(p => {
        if(filtroValor === 'TODOS') return true;
         return String(p.triage) === String(filtroValor);
    });
    if (pacientesFiltrados.length === 0){
        contenedorLista.innerHTML = `<li>No hay pacientes en esta categoria</li>`;
    } else {
        pacientesFiltrados.forEach(p => {
            const li = document.createElement('li');
            li.innerHTML = `<strong>[T${p.triage}]</strong> ${p.nombre} (${p.documento}) - Espera: ${p.minutosEspera} min`;
            contenedorLista.appendChild(li);
        });
    }
    const totales = {1: 0, 2: 0, 3: 0, 4: 0, 5: 0};
    let pacientesTotales = 0;
    listaGlobalPacientes.forEach(p => {
        if(totales[p.triage]!== undefined){
            totales[p.triage]++;
            pacientesTotales++;
        }
    });
    contenedorT1.textContent = "";
    contenedorT2.textContent = "";
    contenedorT3.textContent = "";
    contenedorT4.textContent = "";
    contenedorT5.textContent = "";
    contenedorT1.textContent = `T1: ${totales[1]}`;
    contenedorT2.textContent = `T2: ${totales[2]}`;
    contenedorT3.textContent = `T3: ${totales[3]}`;
    contenedorT4.textContent = `T4: ${totales[4]}`;
    contenedorT5.textContent = `T5: ${totales[5]}`;
    contenedorTotales.textContent = `PACIENTES TOTALES: ${pacientesTotales}`;
}
document.addEventListener("DOMContentLoaded", function() {
    cargarPacientes();
 });
let siguiente = [];
function cargarPaciente(){
    fetch('http://localhost:8080/api/datosPaciente')
        .then(response => response.json())
        .then(data => {
            const nombre = document.getElementById('nombre-consulta');
            const tipoDocumento = document.getElementById('tipoDocumento-consulta');
            const direccion = document.getElementById('direccion-consulta');
            const documento = document.getElementById('documento-consulta');
            const sexo = document.getElementById('sexo-consulta');
            const edad = document.getElementById('edad-consulta');
            const correo = document.getElementById('correo-consulta');
            const telefono = document.getElementById('telefono-consulta');

            if (data.hayPaciente) {
                if (elNombre) elNombre.textContent = data.nombre || "Sin nombre";
                if (elDoc) elDoc.textContent = data.documento || "NA";
                if (elTriage) elTriage.textContent = 'T' + data.triage;
                if (elMotivo) elMotivo.textContent = data.motivo || "NA";
            } else {
                if (elNombre) elNombre.textContent = "No hay pacientes en espera";
                if (elDoc) elDoc.textContent = "-";
                if (elTriage) elTriage.textContent = "-";
                if (elMotivo) elMotivo.textContent = "-";
            }
        })
        .catch(err => {
            console.error("Error al cargar paciente en consulta:", err);
        });
}
function atenderPaciente() {
    fetch('http://localhost:8080/api/atenderPaciente', { 
        method: 'POST' 
})
    .then(response => response.json())
    .then(data => {
       if (data.status === 'ok') {
            cargarPaciente();
            cargarPacientes();
        }else alert("No hay pacientes para atender en la cola.");
    })
    .catch(err => console.error("Error al atender paciente:", err));
}
function deshacerAlta() {
    fetch('http://localhost:8080/api/deshacerAlta', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'ok') {
                cargarPaciente();
                cargarPacientes();
                alert("Se reinsertó el último paciente al sistema.");
            } else alert("El historial de altas está vacío.");
        })
        .catch(err => console.error("Error al deshacer alta:", err));
}
function cargarHistorial(){
    fetch('http://localhost:8080/api/historialPacientes')
        .then(response => response.json())
        .then(data => {
            listaGlobalPacientes = data;
            renderizarListasPacientes();
        })
        .catch(err => console.error("Error al cargar pacientes", err));
}
function renderizarListasPacientes(){
    const contenedorLista = document.getElementById('listaAtendidos');
    if(!contenedorLista) return;
    contenedorLista.innerHTML = '';
    if(listaGlobalPacientes.length === 0) contenedorLista.innerHTML= `<li>No hay pacientes en esta categoria</li>`;
    else {
        listaGlobalPacientes.forEach(p => {
            const li = document.createElement('li');
            li.innerHTML = `<strong>[T${p.triage}]</strong> ${p.nombre} (${p.documento}) - Espera: ${p.minutosEspera} min`;
            contenedorLista.appendChild(li);
        });
    }
}