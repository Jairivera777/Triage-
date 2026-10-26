#include "backend.h"
bool hayPacienteConsulta = false;
Triage pacienteConsulta;
// inicia el server
void inicializarServidor(httplib::Server& svr) {
    servidorGlobal = &svr;
    std::signal(SIGINT, manejarSignal);

    //se guerda la info del paciente
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

    //Se devuelve un json con todos los pacientes
    svr.Get("/api/pacientes", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!colaTriage.isVacia()) {
            Cola cola;
            Nodo * actual;
            Triage triage;
            Paciente * paciente;
            string nom, doc;
            string json = "[";
            bool isPrimero = true;
            for (int i = 0; i<5; i++){
                if(colaTriage.getCola(i).isVacia()) continue;
                cola = colaTriage.getCola(i);
                actual = cola.getCabeza();
                while(actual!=NULL){
                    triage = actual->dato;
                    paciente = triage.getPaciente();
                    nom = (paciente != NULL) ? paciente->getNombre() : "Sin nombre";
                    doc = (paciente != NULL) ? paciente->getDocumento() : "NA";
                    if(!isPrimero) json += ",";
                    json += "{";
                    json += "\"nombre\": \"" + nom + "\",";
                    json += "\"documento\": \"" + doc + "\",";
                    json += "\"triage\": " + to_string(triage.getTriage()) + ",";
                    json += "\"minutosEspera\": " + to_string(triage.getMinutosEspera()) + "";
                    json += "}";
                    actual = actual->siguiente;
                    isPrimero = false;
                }
            }
            json += "]";
            res.set_content(json, "application/json");
        }else res.set_content("[]", "application/json");
    });
    //devuelve al front un json con los datos del paciente
    svr.Get("/api/datosPaciente", [&svr](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if(!hayPacienteConsulta && !colaTriage.isVacia()){
            pacienteConsulta = colaTriage.atender();
            hayPacienteConsulta = true;
        }
        if (hayPacienteConsulta) {
            Paciente* paciente = pacienteConsulta.getPaciente();
            string json = "{";
            json += "\"hayPaciente\": true,";
            json += "\"nombre\": \"" + paciente->getNombre() + "\",";
            json += "\"tipoDocumento\": \"" + paciente->getTipoDocumento() + "\",";
            json += "\"documento\": \"" + paciente->getDocumento() + "\",";
            json += "\"sexo\": \"" + paciente->getSexo() + "\",";
            json += "\"edad\": " + to_string(paciente->getEdad()) + ",";
            json += "\"eps\": \"" + paciente->getEps() + "\",";
            json += "\"correo\": \"" + paciente->getCorreo() + "\",";
            json += "\"telefono\": \"" + paciente->getTelefono() + "\",";
            json += "\"embarazo\": ";
            if (pacienteConsulta.getEmbarazo()) json += "\"Si\",";
            else json += "\"No\",";
            json += "\"temperatura\": " + to_string(pacienteConsulta.getTemperatura()) + ",";
            json += "\"estatura\": " + to_string(pacienteConsulta.getEstatura()) + ",";
            json += "\"peso\": " + to_string(pacienteConsulta.getPeso()) + ",";
            json += "\"frecuenciaCardiaca\": " + to_string(pacienteConsulta.getFrecuenciaCardiaca()) + ",";
            json += "\"presionSistolica\": " + to_string(pacienteConsulta.getPresionSistolica()) + ",";
            json += "\"presionDiastolica\": " + to_string(pacienteConsulta.getPresionDiastolica()) + ",";
            json += "\"frecuenciaRespiratoria\": " + to_string(pacienteConsulta.getFrecuenciaRespiratoria()) + ",";
            json += "\"saturacion\": " + to_string(pacienteConsulta.getSaturacion()) + ",";
            json += "\"glasgow\": " + to_string(pacienteConsulta.getGlasgow()) + ",";
            json += "\"escala\": " + to_string(pacienteConsulta.getEscala()) + ",";
            json += "\"motivo\": \"" + pacienteConsulta.getMotivo() + "\",";
            json += "\"alergias\": \"" + pacienteConsulta.getAlergias() + "\",";
            json += "\"enfermedades\": \"" + pacienteConsulta.getEnfermedades() + "\",";
            json += "\"cirugias\": \"" + pacienteConsulta.getCirugias() + "\",";
            json += "\"observaciones\": \"" + pacienteConsulta.getObservaciones() + "\"";
            json += "}";
            res.set_content(json, "application/json");
        } else {
            res.set_content("{\"hayPaciente\": false, \"nombre\": \"No hay pacientes en espera\"}", "application/json");
        }
    });

    //Atiende al paciente
    svr.Post("/api/atenderPaciente", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (hayPacienteConsulta) {
            pilaAlta.agregarPacienteAtendido(pacienteConsulta);
            hayPacienteConsulta = false;
            res.set_content("{\"status\": \"ok\"}", "application/json");
        } else {
            res.set_content("{\"status\": \"vacio\"}", "application/json");
        }
    });

    //deshace el ultimo alta por si el medico hace miss click
    svr.Post("/api/deshacerAlta", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            Triage recuperado = pilaAlta.deshacerAccion();
            if(hayPacienteConsulta) colaTriage.agregar(pacienteConsulta);
            pacienteConsulta = recuperado;
            hayPacienteConsulta = true;
            res.set_content("{\"status\": \"ok\"}", "application/json");
        } catch (...) {
            res.set_content("{\"status\": \"vacio\"}", "application/json");
        }
    });

    //devuelve al front un Json con los datos mas relevantes para que el medico lo vea.
    svr.Get("/api/proximoPaciente", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        if (!colaTriage.isVacia() && colaTriage.getPrimero()!=NULL) {
            Nodo * actual = colaTriage.getPrimero();
            Triage triage = actual->dato;
            Paciente * paciente = triage.getPaciente();
            string json = "{";
            string nom = (paciente != NULL) ? paciente->getNombre() : "Sin nombre";
            string doc = (paciente != NULL) ? paciente->getDocumento() : "NA";
            string tipoDoc = (paciente!=NULL) ? paciente->getTipoDocumento() : "SI";
            json += "\"hayPaciente\": true,";
            json += "\"nombre\": \"" + nom + "\",";
            json += "\"edad\": \"" + to_string(paciente->getEdad()) +"\",";
            json += "\"documento\": \"" + doc + "\",";
            json += "\"tipoDocumento\": \"" + tipoDoc +"\",";
            json += "\"sexo\": \"" + paciente->getSexo() + "\",";
            json += "\"estatura\": \"" + to_string(triage.getEstatura()) + "\",";
            json += "\"peso\": \"" + to_string(triage.getPeso()) + "\"";
            json += "}";
            res.set_content(json, "application/json");
        } else {
            res.set_content("{\"hayPaciente\": false}", "application/json");
        }
    });

    //cierra el server
    svr.Get("/api/stop", [&svr](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("{\"status\":\"servidor deteniendose\"}", "application/json");
        svr.stop();
    });
}
