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
const selectSexoConsulta = document.getElementById('sexo-consulta');
const seccionEmbarazoConsulta = document.getElementById('seccion-embarazo-consulta');
selectSexoConsulta.addEventListener('change', function(){
    if(this.value === 'Femenino'){
        seccionEmbarazoConsulta.style.display = 'block';
    }else{
        seccionEmbarazoConsulta.style.display = 'none'
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
            li.classList.add('triage-' + p.triage);
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
            const isPrenada = document.getElementById('verdadero-consulta');
            const noPrenada = document.getElementById('falsa-consulta');
            const edad = document.getElementById('edad-consulta');
            const correo = document.getElementById('correo-consulta');
            const telefono = document.getElementById('telefono-consulta');
            const temperatura = document.getElementById('temperatura-consulta');
            const estatura = document.getElementById('altura-consulta');
            const peso = document.getElementById('peso-consulta');
            const frecuenciaCardiaca = document.getElementById('frecuencia-consulta');
            const presionSistolica = document.getElementById('presionSistolica-consulta');
            const presionDiastolica = document.getElementById('presionDiastolica-consulta');
            const frecuenciaRespiratoria = document.getElementById('frecuenciaRespiratoria-consulta');
            const saturacion = document.getElementById('saturacion-consulta');
            const glasgow = document.getElementById('escalaGlasgow-consulta');
            const escala = document.getElementById('escala-consulta');
            const motivo = document.getElementById('motivo-consulta');
            const alergias = document.getElementById('alergias-consulta');
            const enfermedades = document.getElementById('enfermedades-consulta');
            const cirugias = document.getElementById('cirugias-consulta');
            const observaciones = document.getElementById('observaciones-consulta');
            if (data.hayPaciente) {
                if (nombre) nombre.value = data.nombre || "Sin nombre";
                if (tipoDocumento) tipoDocumento.textContent = data.tipoDocumento+":" || "SI";
                if (documento) documento.value = data.document || "NA";
                if (direccion) direccion.value = data.direccion || "NA";
                if (sexo){
                    if(data.sexo==='Femenino') {
                        sexo.value = 'Femenino';
                    }
                    else sexo.value = 'Masculino';
                } 
                const seccionEmbarazo = document.getElementById('seccion-embarazo-consulta');
                
                if(sexo && sexo.value === 'Femenino'){
                    seccionEmbarazo.style.display = 'block';
                    
                    const condicion = (data.embarazo === "Si"); 
                    if(isPrenada) isPrenada.checked = condicion;
                    if(noPrenada) noPrenada.checked = !condicion;
                } else {
                    seccionEmbarazo.style.display = 'none';
                    if(noPrenada) noPrenada.checked = true;
                }
                if (edad) edad.value = data.edad || 0;
                if (correo) correo.value = data.correo || "NA";
                if (telefono) telefono.value = data.telefono || "NA";
                if(temperatura) temperatura.value = data.temperatura || 0;
                if (estatura) estatura.value = data.estatura || 0;
                if (peso) peso.value = data.peso || 0;
                if (frecuenciaCardiaca) frecuenciaCardiaca.value = data.frecuenciaCardiaca || 0;
                if (presionSistolica) presionSistolica.value = data.presionSistolica || 0;
                if (presionDiastolica) presionDiastolica.value = data.presionDiastolica || 0;
                if(frecuenciaRespiratoria) frecuenciaRespiratoria.value = data.frecuenciaRespiratoria || 0;
                if (saturacion) saturacion.value = data.saturacion || 0;
                if (glasgow) glasgow.value = data.glasgow || 15;
                if(escala) escala.value = data.escala || 1;
                if (motivo) motivo.value = (!data.motivo || data.motivo === "-") ? "NA" : data.motivo;
                if (alergias) alergias.value = (!data.alergias || data.alergias === "-") ? "NA" : data.alergias;
                if (enfermedades) enfermedades.value = (!data.enfermedades || data.enfermedades === "-") ? "NA" : data.enfermedades;
                if (cirugias) cirugias.value = (!data.cirugias || data.cirugias === "-") ? "NA" : data.cirugias; 
                if (observaciones) observaciones.value = (!data.observaciones || data.observaciones === "-") ? "NA" : data.observaciones;
            } else {
                if (nombre) nombre.value = data.nombre;
                if (tipoDocumento) tipoDocumento.textContent = "-";
                if (documento) documento.value = "-";
                if (direccion) direccion.value = "-";
                if (sexo) sexo.selectedIndex = 0;
                if (edad) edad.value = 0;
                if (correo) correo.value = "-";
                if (telefono) telefono.value = "-";
                if(temperatura) temperatura.value = 0;
                if (estatura) estatura.value = 0;
                if (peso) peso.value = 0;
                if (frecuenciaCardiaca) frecuenciaCardiaca.value = 0;
                if (presionSistolica) presionDiastolica.value = 0;
                if(frecuenciaRespiratoria) frecuenciaRespiratoria.value = 0;
                if (saturacion) saturacion.value = 0;
                if (glasgow) glasgow.value = 15;
                if(escala) escala.value = 1;
                if (motivo) motivo.value = "-";
                if (alergias) alergias.value = "-";
                if (enfermedades) enfermedades.value = "-";
                if (cirugias) enfermedades.value = "-"; 
                if (observaciones) observaciones.value = "-";
            }
            cargarProximoPaciente();
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
function cargarProximoPaciente(){
    fetch('http://localhost:8080/api/proximoPaciente')
        .then(response => response.json)
        .then(data => {
            const nom = document.getElementById('siguientePaciente');
            const tipo = document.getElementById('siguienteTipoDocumento');
            const doc = document.getElementById('siguienteDocumento');
            const edad = document.getElementById('edadSiguiente');
            const sexo = document.getElementById('sexoSiguiente');
            const estatura = document.getElementById('estaturaSiguiente');
            const peso = document.getElementById('pesoSiguiente');
            if(data.hayPaciente){
                if (nom) nom.textContent = data.nombre || "NA";
                if (tipo) tipo.textContent = data.tipoDocumento+":" || "SI:";
                if (doc) doc.textContent = data.documento || "NA";
                if (edad) edad.textContent = data.edad || "NA";
                if (sexo) sexo.textContent = data.sexo;
                if (estatura) estatura.textContent = data.estatura || "NA";
                if (peso) peso.textContent = data.peso || "NA";
            }else{
                if (nom) nom.textContent = "Sin siguiente paciente";
                if (tipo) tipo.textContent = "SI:";
                if (doc) doc.textContent = "-";
                if (edad) edad.textContent = "-";
                if (sexo) sexo.textContent = "-";
                if (estatura) estatura.textContent = "-";
                if (peso) peso.textContent = "-";
            }
        })
        .catch(err => console.error("Error al cargar el proximo paciente"));
}
