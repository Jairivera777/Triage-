#include "backend.h"

// Este archivo centraliza todas las rutas HTTP del servidor.
// Mantenerlas aquí hace que el punto de entrada del programa sea mucho más limpio.
// Cada endpoint responde a una acción concreta de la interfaz con el backend.

// Registra todas las rutas del backend con la interfaz
void inicializarServidor(httplib::Server& svr) {
    servidorGlobal = &svr;
    std::signal(SIGINT, manejarSignal);

    // Endpoint para crear un nuevo triage desde la interfaz.
    // La información del paciente, signos vitales y motivo de consulta se envían aquí.
    svr.Post("/api/triage", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        Paciente* paciente = NULL;
        string tipoDocumentoPaciente = req.tieneParametro("tipoDocumentoPaciente") ? req.getParametro("tipoDocumentoPaciente") : "SI";
        string sexo = req.tieneParametro("sexo") ? req.getParametro("sexo") : "NA";
        string documentoPaciente = "NA";

        if (tipoDocumentoPaciente == "AS" || tipoDocumentoPaciente == "MS" || tipoDocumentoPaciente == "SI") {
            paciente = new Paciente(sexo, tipoDocumentoPaciente);
        } else {
            string nombrePaciente = req.tieneParametro("nombrePaciente") ? req.getParametro("nombrePaciente") : "Sin nombre";
            documentoPaciente = req.tieneParametro("documentoPaciente") ? req.getParametro("documentoPaciente") : "NA";
            int edad = (req.tieneParametro("edad") && !req.getParametro("edad").empty()) ? stoi(req.getParametro("edad")) : 0;
            string eps = req.tieneParametro("eps") ? req.getParametro("eps") : "NA";
            string correoPaciente = req.tieneParametro("correoPaciente") ? req.getParametro("correoPaciente") : "NA";
            string telefonoPaciente = req.tieneParametro("telefono") ? req.getParametro("telefono") : "NA";
            string direccion = req.tieneParametro("direccion") ? req.getParametro("direccion") : "NA";
            paciente = new Paciente(nombrePaciente, documentoPaciente, tipoDocumentoPaciente, edad, sexo, eps, correoPaciente, telefonoPaciente, direccion);
        }

        cout << "\n====================================" << endl;
        cout << "NUEVA PETICION DE TRIAGE RECIBIDA!" << endl;
        cout << "Paciente de " << tipoDocumentoPaciente << ": " << documentoPaciente << " de sexo: " << sexo << endl;

        AcudientePaciente* acudiente = NULL;
        string parentesco = req.tieneParametro("parentesco") ? req.getParametro("parentesco") : "NA";
        if (parentesco != "nadie" && parentesco != "NA") {
            string nombreAcudiente = req.tieneParametro("nombreAcudiente") ? req.getParametro("nombreAcudiente") : "NA";
            string tipoDocumentoAcudiente = req.tieneParametro("tipoDocumentoAcudiente") ? req.getParametro("tipoDocumentoAcudiente") : "SI";
            string documentoAcudiente = req.tieneParametro("documentoAcudiente") ? req.getParametro("documentoAcudiente") : "NA";
            string correoAcudiente = req.tieneParametro("correoAcudiente") ? req.getParametro("correoAcudiente") : "NA";
            string telefonoAcudiente = req.tieneParametro("telefonoAcudiente") ? req.getParametro("telefonoAcudiente") : "NA";
            acudiente = new AcudientePaciente(nombreAcudiente, tipoDocumentoAcudiente, documentoAcudiente, parentesco, correoAcudiente, telefonoAcudiente, paciente);
        }

        int temperatura = (req.tieneParametro("temperatura") && !req.getParametro("temperatura").empty()) ? stof(req.getParametro("temperatura")) : 36;
        bool embarazo = req.tieneParametro("embarazada") ? req.pasarBool(req.getParametro("embarazada")) : false;
        int estatura = (req.tieneParametro("estatura") && !req.getParametro("estatura").empty()) ? stoi(req.getParametro("estatura")) : 160;
        float peso = (req.tieneParametro("peso") && !req.getParametro("peso").empty()) ? stof(req.getParametro("peso")) : 60;
        int presionSistolica = (req.tieneParametro("presionSistolica") && !req.getParametro("presionSistolica").empty()) ? stoi(req.getParametro("presionSistolica")) : 100;
        int presionDiastolica = (req.tieneParametro("presionDiastolica") && !req.getParametro("presionDiastolica").empty()) ? stoi(req.getParametro("presionDiastolica")) : 90;
        int frecuenciaCardiaca = (req.tieneParametro("frecuencia") && !req.getParametro("frecuencia").empty()) ? stoi(req.getParametro("frecuencia")) : 60;
        int frecuenciaRespiratoria = (req.tieneParametro("frecuenciaRespiratoria") && !req.getParametro("frecuenciaRespiratoria").empty()) ? stoi(req.getParametro("frecuenciaRespiratoria")) : 15;
        int saturacion = (req.tieneParametro("saturacion") && !req.getParametro("saturacion").empty()) ? stoi(req.getParametro("saturacion")) : 90;
        int escalaDolor = (req.tieneParametro("escala") && !req.getParametro("escala").empty()) ? stoi(req.getParametro("escala")) : 0;
        int escalaGlasgow = (req.tieneParametro("escalaGlasgow") && !req.getParametro("escalaGlasgow").empty()) ? stoi(req.getParametro("escalaGlasgow")) : 15;
        string motivo = req.tieneParametro("motivo") ? req.getParametro("motivo") : "NA";
        string alergias = req.tieneParametro("alergias") ? req.getParametro("alergias") : "NA";
        string enfermedades = req.tieneParametro("enfermedades") ? req.getParametro("enfermedades") : "NA";
        string cirugias = req.tieneParametro("cirugias") ? req.getParametro("cirugias") : "NA";
        string observaciones = req.tieneParametro("observaciones") ? req.getParametro("observaciones") : "NA";

        Triage triage(paciente, embarazo, temperatura, estatura, peso, presionSistolica,
                      presionDiastolica, frecuenciaCardiaca, frecuenciaRespiratoria,
                      saturacion, escalaDolor, motivo, alergias, enfermedades, cirugias,
                      observaciones, escalaGlasgow, contadorTurno++);
        colaTriage.agregar(triage);

        int nivelCalculado = triage.getTriage();
        cout << "Triage calculado exitosamente: NIVEL " << nivelCalculado << endl;
        cout << "=======================================" << endl;

        string jsonRespuesta = "{\"status\":\"ok\", \"triage\":" + to_string(nivelCalculado) + "}";
        res.set_content(jsonRespuesta, "application/json");
    });

    // Endpoint para devolver la lista ordenada de pacientes en espera.
    // Se toma la cola general y se ordena por prioridad antes de formatear el JSON.
    svr.Get("/api/pacientes", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        Lista listaOrdenada;
        for (int i = 0; i < colaTriage.longitud(); i++) {
            listaOrdenada.insertarOrdenado(colaTriage.obtener(i));
        }

        string json = "[";
        Nodo* actual = listaOrdenada.getCabeza();
        bool primero = true;
        while (actual != NULL) {
            Triage t = actual->dato;
            if (!primero) json += ",";
            string doc = "NA";
            string nom = "Anonimo";

            if (t.getPaciente() != NULL) {
                if (!t.getPaciente()->getDocumento().empty()) doc = t.getPaciente()->getDocumento();
                if (!t.getPaciente()->getNombre().empty()) nom = t.getPaciente()->getNombre();
            }

            json += "{";
            json += "\"documento\":\"" + doc + "\",";
            json += "\"nombre\":\"" + nom + "\",";
            json += "\"triage\":" + to_string(t.getTriage()) + ",";
            json += "\"minutosEspera\":" + to_string(t.obtenerMinutosEspera());
            json += "}";
            primero = false;
            actual = actual->siguiente;
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    // Endpoint para consultar los datos completos del paciente más prioritario.
    // Sirve para llenar la vista del paciente actual que está en espera.
    svr.Get("/api/datosPaciente", [&svr](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!colaTriage.esvacia()) {
            Triage cabeza = colaTriage.obtener(0);
            Paciente* paciente = cabeza.getPaciente();
            string json = "{";
            json += "\"hayPaciente\": true,";
            json += "\"nombre\": \"" + paciente->getNombre() + "\",";
            json += "\"tipoDocumento\": \"" + paciente->getTipoDocumento() + "\",";
            json += "\"documento\": \"" + paciente->getDocumento() + "\",";
            json += "\"sexo\": \"" + paciente->getSexo() + "\",";
            json += "\"edad\": \"" + to_string(paciente->getEdad()) + "\",";
            json += "\"eps\": \"" + paciente->getEps() + "\",";
            json += "\"correo\": \"" + paciente->getCorreo() + "\",";
            json += "\"telefono\": \"" + paciente->getTelefono() + "\",";
            json += "\"embarazo\": ";
            if (cabeza.getEmbarazo()) json += "\"Si\",";
            else json += "\"No\",";
            json += "\"temperatura\": \"" + to_string(cabeza.getTemperatura()) + "\",";
            json += "\"estatura\": \"" + to_string(cabeza.getEstatura()) + "\",";
            json += "\"peso\": \"" + to_string(cabeza.getPeso()) + "\",";
            json += "\"frecuenciaCardiaca\": \"" + to_string(cabeza.getFrecuenciaCardiaca()) + "\",";
            json += "\"presionSistolica\": \"" + to_string(cabeza.getPresionSistolica()) + "\",";
            json += "\"presionDiastolica\": \"" + to_string(cabeza.getPresionDiastolica()) + "\",";
            json += "\"frecuenciaRespiratoria\": \"" + to_string(cabeza.getFrecuenciaRespiratoria()) + "\",";
            json += "\"saturacion\": \"" + to_string(cabeza.getSaturacion()) + "\",";
            json += "\"glasgow\": \"" + to_string(cabeza.getGlasgow()) + "\",";
            json += "\"escala\": \"" + to_string(cabeza.getEscala()) + "\",";
            json += "\"motivo\": \"" + cabeza.getMotivo() + "\",";
            json += "\"alergias\": \"" + cabeza.getAlergias() + "\",";
            json += "\"enfermedades\": \"" + cabeza.getEnfermedades() + "\",";
            json += "\"cirugias\": \"" + cabeza.getCirugias() + "\",";
            json += "\"observaciones\": \"" + cabeza.getObservaciones() + "\"";
            json += "}";
            res.set_content(json, "application/json");
        } else {
            res.set_content("{\"hayPaciente\": false, \"nombre\": \"No hay pacientes en espera\"}", "application/json");
        }
    });

    // Endpoint para atender al paciente actual y moverlo a la pila de atendidos.
    // Este paso significa que ya fue atendido y ya no está en espera.
    svr.Post("/api/atenderPaciente", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!colaTriage.esvacia()) {
            Triage paciente = colaTriage.eliminar_primero();
            pilaAlta.agregarPacienteAtendido(paciente);
            res.set_content("{\"status\": \"ok\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"vacio\"}", "application/json");
        }
    });

    // Endpoint para deshacer la última atención realizada.
    // Se recupera el paciente de la pila de atendidos y vuelve a la cola.
    svr.Post("/api/deshacerAlta", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            Triage recuperado = pilaAlta.deshacerAccion();
            colaTriage.agregar(recuperado);
            res.set_content("{\"status\": \"ok\"}", "application/json");
        } catch (...) {
            res.set_content("{\"status\": \"vacio\"}", "application/json");
        }
    });

    // Endpoint para devolver el paciente más reciente de la cola como historial básico.
    // Sirve para mostrar información rápida del paciente activo o priorizado.
    svr.Get("/api/historialPacientes", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!colaTriage.esvacia()) {
            Triage cabeza = colaTriage.obtener(0);
            Paciente* paciente = cabeza.getPaciente();

            string nom = (paciente != nullptr) ? paciente->getNombre() : "Sin nombre";
            string doc = (paciente != nullptr) ? paciente->getDocumento() : "NA";

            string json = "{";
            json += "\"nombre\": \"" + escaparJson(nom) + "\",";
            json += "\"documento\": \"" + escaparJson(doc) + "\",";
            json += "\"triage\": " + to_string(cabeza.getTriage()) + ",";
            json += "\"motivo\": \"" + escaparJson(cabeza.getMotivo()) + "\"";
            json += "}";
            res.set_content(json, "application/json");
        } else {
            res.set_content("{\"hayPaciente\": false, \"nombre\": \"No hay pacientes en espera\"}", "application/json");
        }
    });

    // Endpoint de apagado del servidor.
    // Se usa para detener la app de forma controlada desde la interfaz o el navegador.
    svr.Get("/api/stop", [&svr](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("{\"status\":\"servidor deteniendose\"}", "application/json");
        svr.stop();
    });
}
