#include "backend.h"
using namespace std;
httplib::Server* servidorGlobal = nullptr;

void manejarSignal(int signal) {
    if (servidorGlobal) {
        cout << "Cerrando servidor mediante Ctrl+C" << endl;
        servidorGlobal->stop();
    }
}

string escaparJson(string texto) {
    string limpio = "";
    for (size_t i = 0; i < texto.length(); i++) {
        char c = texto[i];
        if (c == '"') limpio += "\\\"";
        else if (c == '\\') limpio += "\\\\";
        else if (c == '\n') limpio += "\\n";
        else if (c == '\r') continue;
        else limpio += c;
    }
    return limpio;
}

Paciente::Paciente() {}

Paciente::Paciente(string _sexo, string _tipoDocumento) {
    nombre = "NA";
    tipoDocumento = _tipoDocumento;
    sexo = _sexo;
    documento = "NA";
    edad = 0;
    correo = "NA";
    telefono = "NA";
    direccion = "NA";
    eps = "NA";
}

Paciente::Paciente(string _nombre, string _documento, string _tipoDocumento, int _edad,
                   string _sexo, string _eps, string _correo, string _telefono,
                   string _direccion) {
    nombre = _nombre;
    documento = _documento;
    tipoDocumento = _tipoDocumento;
    edad = _edad;
    sexo = _sexo;
    eps = _eps;
    correo = _correo;
    telefono = _telefono;
    direccion = _direccion;
}

string Paciente::getSexo() { return sexo; }
int Paciente::getEdad() { return edad; }
string Paciente::getDocumento() { return documento; }
string Paciente::getNombre() { return nombre; }
string Paciente::getTipoDocumento() { return tipoDocumento; }
string Paciente::getEps() { return eps; }
string Paciente::getCorreo() { return correo; }
string Paciente::getTelefono() { return telefono; }
string Paciente::getDireccion() { return direccion; }

AcudientePaciente::AcudientePaciente(string _nombre, string _tipoDocumento,
                                     string _documento, string _parentesco,
                                     string _correo, string _telefono,
                                     Paciente* _paciente) {
    nombre = _nombre;
    tipoDocumento = _tipoDocumento;
    documento = _documento;
    parentesco = _parentesco;
    correo = _correo;
    telefono = _telefono;
    paciente = _paciente;
}

string AcudientePaciente::getNombre() { return nombre; }
string AcudientePaciente::getTipoDocumento() { return tipoDocumento; }
string AcudientePaciente::getDocumento() { return documento; }
string AcudientePaciente::getParentesco() { return parentesco; }
string AcudientePaciente::getCorreo() { return correo; }
string AcudientePaciente::getTelefono() { return telefono; }

bool Triage::buscarPalabras(string texto, string palabras[], int tamano) {
    for (int i = 0; i < tamano; i++) {
        if (texto.find(palabras[i]) != string::npos) {
            return true;
        }
    }
    return false;
}

string Triage::normalizar(string palabra) {
    for (char& c : palabra) {
        c = toupper(c);
    }

    struct Reemplazo { string conTilde; string sinTilde; };
    Reemplazo lista[] = {
        {"Á", "A"}, {"É", "E"}, {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"},
        {"á", "A"}, {"é", "E"}, {"í", "I"}, {"ó", "O"}, {"ú", "U"},
        {"ü", "U"}, {"Ü", "U"}
    };

    for (const auto& r : lista) {
        size_t pos = 0;
        while ((pos = palabra.find(r.conTilde, pos)) != string::npos) {
            palabra.replace(pos, r.conTilde.length(), r.sinTilde);
            pos += r.sinTilde.length();
        }
    }
    return palabra;
}

// Calcula el nivel de triage según signos vitales y síntomas
int Triage::calcularTriage() {
    tiempoEspera = obtenerMinutosEspera();
    string texto = motivo + " " + observaciones;

    string palabrasNivel1[] = {"DISNEA DE REPOSO", "HABLA ENTRECORTADA", "USO MARCADO DE MUSCULOS ACCESORIOS", "CIANOSIS DISTAL",
        "CIANOSIS PERIORAL", "ALTERACION DEL ESTADO DE ALERTA", "TRAUMA SEVERO", "POLITRAUMATISMO CRITICO", "PERDIDA DE CONSCIENCIA PROFUNDA",
        "COMA", "ESTUPOR", "ACV", "ACCIDENTE CEREBRO VASCULAR", "HEMORRAGIA SEVERA", "QUEMADURAS EXTENSAS", "RIESGO DE PERDER UN MIEMBRO",
        "RIESGO DE PERDER UN ORGANO", "SHOCK", "PERDIDA DE CONCIENCIA PROFUNDA", "PARO RESPIRATORIO", "APNEA", "INSUFICIENCIA RESPIRATORIA AGUDA",
        "PARO CARDIORESPIRATORIO", "PCR", "DOLOR TORACICO OPRESIVO", "INFARTO", "IAM", "HEMORRAGIA EXANGUINANTE", "CONVULSION", "ESTADO EPILEPTICO"};
    int tamano1 = sizeof(palabrasNivel1) / sizeof(palabrasNivel1[0]);

    string palabrasNivel2[] = {"DOLOR MUY INTENSO", "DOLOR EN EL PECHO", "COLICOS NEFRITICOS", "INTOXICACION AGUDA", "ENVENENAMIENTO", "HEMORRAGIAS",
        "FIEBRE ALTA", "DIFICULTAD RESPIRATORIA MODERADA", "TRAUMATISMO", "LESIONES GRAVES"};
    int tamano2 = sizeof(palabrasNivel2) / sizeof(palabrasNivel2[0]);

    string epoc[] = {"EPOC", "ENFERMEDAD PULMONAR CRONICA"};
    bool encontroEpoc = buscarPalabras(observaciones, epoc, 2);

    string debilidad[] = {"DEBILIDAD EXTREMA", "CONFUSION"};
    string palabrasNivel3[] = {"DOLOR MODERADO", "VOMITO PERSISTENTE", "DIARREA RECUENTE", "DESHIDRATACION MODERADA", "HERIDA PARA SUTURA",
        "SUTURA", "REACCION ALERGICA LEVE", "ESGUINCE", "SOSPECHA DE FRACTURA", "TRAUMA MODERADO", "LUXACION", "RETENCION URINARIA",
        "DOLOR ABDOMINAL MODERADO", "CRISIS ASMATICA LEVE", "HEMOPTISIS LEVE", "SANGRE EN LA ORINA", "HEMATURIA", "REACCION ADVERSA A MEDICAMENTO",
        "HISTERIA", "CRISIS DE ANSIEDAD"};
    int tamano3 = sizeof(palabrasNivel3) / sizeof(palabrasNivel3[0]);

    string factoresRiesgo[] = {"DIABETES", "HIPERTENSION", "INSUFICIENCIA CARDIACA", "CANCER", "INMUNOSUPRESION"};
    string palabrasNivel4[] = {"DOLOR LEVE", "MOLESTIA LEVE", "DOLOR DE VARIOS DIAS", "RESFRIADO COMUN", "RESFRIADO", "DOLOR DE GARGANTA", "CUADRO GRIPAL",
        "GRIPA", "INFECCION URINARIA LEVE", "MOLESTIA AL ORINAR", "DIARREA SIMPLE", "DIARREA SIN DESHIDRATACION", "MOLESTIA DIGESTIVA", "CONTUSION LEVE",
        "GOLPE LEVE", "ESGUINCE MENOR", "HERIDA SUPERFICIAL", "RASPON", "ABSCESO PEQUEÑO", "INFECCION LOCALIZADA"};
    int tamano4 = sizeof(palabrasNivel4) / sizeof(palabrasNivel4[0]);

    int triageNivel = 5;
    if (frecuenciaCardiaca == 0 || buscarPalabras(texto, palabrasNivel1, tamano1) || saturacion < 80 || glasgow <= 8) triageNivel = 1;
    else if (presionSistolica >= 180 || presionDiastolica > 110 || presionSistolica <= 90 || frecuenciaCardiaca >= 120 ||
             frecuenciaCardiaca <= 50 || frecuenciaRespiratoria > 24 || frecuenciaRespiratoria < 10 || (saturacion < 90 && !encontroEpoc)
             || (saturacion <= 85 && encontroEpoc) || temperatura >= 39.5 || (temperatura <= 35 && buscarPalabras(texto, debilidad, 2))
             || buscarPalabras(texto, palabrasNivel2, tamano2) || temperatura <= 34 || escala >= 7) triageNivel = 2;
    else if (temperatura >= 38 || presionSistolica >= 140 || frecuenciaCardiaca >= 100 || frecuenciaRespiratoria >= 21 ||
             buscarPalabras(texto, palabrasNivel3, tamano3) || escala >= 4 ||
             (paciente != NULL && paciente->getEdad() >= 65 && buscarPalabras(enfermedades, factoresRiesgo, 5))) triageNivel = 3;
    else if (buscarPalabras(texto, palabrasNivel4, tamano4)) triageNivel = 4;
    else triageNivel = 5;

    if (triageNivel == 2 && tiempoEspera > 30) triageNivel = 1;
    else if (triageNivel == 3 && tiempoEspera > 100) triageNivel = 1;
    else if (triageNivel == 4 && tiempoEspera > 420) triageNivel = 1;
    else if (triageNivel == 5 && tiempoEspera > 720) triageNivel = 1;

    return triageNivel;
}

Triage::Triage() {
    paciente = nullptr;
    embarazo = false;
    temperatura = 36;
    estatura = 160;
    peso = 60;
    presionSistolica = 120;
    presionDiastolica = 80;
    frecuenciaCardiaca = 75;
    frecuenciaRespiratoria = 18;
    saturacion = 98;
    escala = 0;
    glasgow = 15;
    horaLlegada = time(0);
    turnoLlegada = 0;
    triage = 5;
}

Triage::Triage(Paciente* _paciente, bool _embarazo, float _temperatura, int _estatura, float _peso,
               int _presionSistolica, int _presionDiastolica, int _frecuenciaCardiaca,
               int _frecuenciaRespiratoria, int _saturacion, int _escala, string _motivo,
               string _alergias, string _enfermedades, string _cirugias,
               string _observaciones, int _glasgow, int _turno, AcudientePaciente* _acudiente) {
    paciente = _paciente;
    acudiente = _acudiente;
    if (paciente != nullptr && paciente->getSexo() == "Masculino") embarazo = 0;
    else embarazo = _embarazo;
    temperatura = _temperatura;
    estatura = _estatura;
    peso = _peso;
    presionSistolica = _presionSistolica;
    presionDiastolica = _presionDiastolica;
    frecuenciaCardiaca = _frecuenciaCardiaca;
    frecuenciaRespiratoria = _frecuenciaRespiratoria;
    saturacion = _saturacion;
    escala = _escala;
    glasgow = _glasgow;
    motivo = normalizar(_motivo);
    alergias = normalizar(_alergias);
    enfermedades = normalizar(_enfermedades);
    cirugias = normalizar(_cirugias);
    observaciones = normalizar(_observaciones);
    auto ahora = chrono::system_clock::now();
    horaLlegada = chrono::system_clock::to_time_t(ahora);
    triage = calcularTriage();
    turnoLlegada = _turno;
}

AcudientePaciente* Triage::getAcudiente() { return acudiente; }
int Triage::getTriage() {
    triage = calcularTriage();
    return triage;
}
int Triage::getHoraLlegada() { return horaLlegada; }
int Triage::getTurnoLlegada() { return turnoLlegada; }
Paciente* Triage::getPaciente() { return paciente; }
float Triage::getTemperatura() { return temperatura; }
bool Triage::getEmbarazo() { return embarazo; }
int Triage::getEstatura() { return estatura; }
float Triage::getPeso() { return peso; }
int Triage::getPresionSistolica() { return presionSistolica; }
int Triage::getPresionDiastolica() { return presionDiastolica; }
int Triage::getFrecuenciaCardiaca() { return frecuenciaCardiaca; }
int Triage::getFrecuenciaRespiratoria() { return frecuenciaRespiratoria; }
int Triage::getSaturacion() { return saturacion; }
int Triage::getEscala() { return escala; }
int Triage::getGlasgow() { return glasgow; }
string Triage::getMotivo() { return motivo; }
string Triage::getAlergias() { return alergias; }
string Triage::getEnfermedades() { return enfermedades; }
string Triage::getCirugias() { return cirugias; }
string Triage::getObservaciones() { return observaciones; }

int Triage::obtenerMinutosEspera() {
    auto ahora = chrono::system_clock::now();
    time_t horaActual = chrono::system_clock::to_time_t(ahora);
    double segundos = difftime(horaActual, horaLlegada);
    return (int)(segundos / 60);
}

HistoriaClinica::HistoriaClinica() {}
HistoriaClinica::HistoriaClinica(Triage _triage, string _anamnesis, string _diagnostico,
                                 string _tratamiento, string _indicaciones, string _fechaHora) {
    triageIngreso = _triage;
    anamnesis = _anamnesis;
    diagnostico = _diagnostico;
    tratamiento = _tratamiento;
    indicaciones = _indicaciones;
    fechaHoraAtencion = _fechaHora;
}

Triage HistoriaClinica::getTriageIngreso() { return triageIngreso; }
string HistoriaClinica::getAnamnesis() { return anamnesis; }
string HistoriaClinica::getDiagnostico() { return diagnostico; }
string HistoriaClinica::getTratamiento() { return tratamiento; }
string HistoriaClinica::getIndicaciones() { return indicaciones; }
string HistoriaClinica::getFechaHoraAtencion() { return fechaHoraAtencion; }

NodoHistoria::NodoHistoria(HistoriaClinica h) {
    dato = h;
    siguiente = NULL;
}

ListaHistorias::ListaHistorias() {
    cabeza = NULL;
    ultimo = NULL;
}

ListaHistorias::~ListaHistorias() {
    NodoHistoria* actual = cabeza;
    while (actual != NULL) {
        NodoHistoria* sig = actual->siguiente;
        delete actual;
        actual = sig;
    }
}

void ListaHistorias::insertarAlFinal(HistoriaClinica h) {
    NodoHistoria* nuevo = new NodoHistoria(h);
    if (cabeza == NULL) {
        cabeza = nuevo;
        ultimo = cabeza;
    } else {
        ultimo->siguiente = nuevo;
        ultimo = nuevo;
    }
}

NodoHistoria* ListaHistorias::getCabeza() { return cabeza; }

bool esMenor(Triage& a, Triage& b) {
    if (a.getTriage() != b.getTriage()) return a.getTriage() < b.getTriage();
    if (a.getHoraLlegada() != b.getHoraLlegada()) return a.getHoraLlegada() < b.getHoraLlegada();
    return a.getTurnoLlegada() < b.getTurnoLlegada();
}

Nodo::Nodo(Triage t) {
    dato = t;
    siguiente = NULL;
}

PilaAtendidos::PilaAtendidos() {
    cabeza = NULL;
    cantidad = 0;
}

void PilaAtendidos::agregarPacienteAtendido(Triage t) {
    Nodo* nuevo = new Nodo(t);
    if (cabeza == NULL) {
        cabeza = nuevo;
        nuevo->siguiente = NULL;
        cantidad = 1;
        return;
    }
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
    cantidad++;
}

Triage PilaAtendidos::deshacerAccion() {
    Nodo* temp = cabeza;
    if (cabeza == NULL) throw -1;
    if (cantidad == 1) {
        Triage paciente = temp->dato;
        delete temp;
        cabeza = NULL;
        cantidad = 0;
        return paciente;
    }
    Triage paciente = temp->dato;
    cabeza = cabeza->siguiente;
    delete temp;
    return paciente;
}

Nodo* PilaAtendidos::getTope() { return cabeza; }

Cola::Cola() {
    cabeza = NULL;
    ultimo = NULL;
}

bool Cola::isVacia() { return cabeza == NULL; }

void Cola::insertarUltimo(Triage t) {
    Nodo* nuevo = new Nodo(t);
    if (isVacia()) {
        cabeza = nuevo;
        ultimo = cabeza;
        return;
    }
    ultimo->siguiente = nuevo;
    ultimo = nuevo;
}

void Cola::insertarPrimero(Triage t) {
    Nodo* nuevo = new Nodo(t);
    if (cabeza == NULL) {
        cabeza = nuevo;
        ultimo = cabeza;
        return;
    }
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
}

Triage Cola::atenderPaciente() {
    if (isVacia()) throw -1;
    Nodo* aux = cabeza;
    Triage paciente = aux->dato;
    cabeza = cabeza->siguiente;
    aux->siguiente = NULL;
    if (cabeza == NULL) ultimo = NULL;
    delete aux;
    return paciente;
}

Nodo* Cola::getCabeza() { return cabeza; }

Lista::Lista() {
    cabeza = NULL;
    ultimo = NULL;
}

void Lista::insertarOrdenado(Triage t) {
    Nodo* nuevo = new Nodo(t);
    if (cabeza == NULL) {
        cabeza = nuevo;
        ultimo = nuevo;
        return;
    }
    if (esMenor(t, cabeza->dato)) {
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        return;
    }
    Nodo* actual = cabeza;
    while (actual->siguiente != NULL && !esMenor(t, actual->siguiente->dato)) {
        actual = actual->siguiente;
    }
    nuevo->siguiente = actual->siguiente;
    actual->siguiente = nuevo;
    if (nuevo->siguiente == NULL) ultimo = nuevo;
}

Nodo* Lista::getCabeza() { return cabeza; }

ColaPrioridad::ColaPrioridad() {}

void ColaPrioridad::agregar(Triage t) {
    int prioridad = t.getTriage();
    colas[prioridad - 1].insertarUltimo(t);
}

void ColaPrioridad::deshacer(Triage t) {
    int prioridad = t.getTriage();
    colas[prioridad - 1].insertarPrimero(t);
}

Triage ColaPrioridad::atender() {
    for (int i = 0; i < 5; i++) {
        if (!colas[i].isVacia()) return colas[i].atenderPaciente();
    }
    throw -1;
}

bool ColaPrioridad::isVacia() {
    for (int i = 0; i < 5; i++) {
        if (!colas[i].isVacia()) return false;
    }
    return true;
}

bool ColaPrioridad::esvacia() { return isVacia(); }

int ColaPrioridad::longitud() {
    int total = 0;
    for (int i = 0; i < 5; i++) {
        Nodo* actual = colas[i].getCabeza();
        while (actual != NULL) {
            total++;
            actual = actual->siguiente;
        }
    }
    return total;
}

Triage ColaPrioridad::obtener(int indice) {
    int actual = 0;
    for (int i = 0; i < 5; i++) {
        Nodo* n = colas[i].getCabeza();
        while (n != NULL) {
            if (actual == indice) return n->dato;
            actual++;
            n = n->siguiente;
        }
    }
    throw -1;
}

Triage ColaPrioridad::eliminar_primero() {
    for (int i = 0; i < 5; i++) {
        if (!colas[i].isVacia()) return colas[i].atenderPaciente();
    }
    throw -1;
}

int contadorTurno = 1;
ColaPrioridad colaTriage;
PilaAtendidos pilaAlta;

