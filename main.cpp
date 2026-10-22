#include "httplib.h"
#include <iostream>
#include <string>
#include <chrono>
#include <csignal>
using namespace std;
httplib::Server* servidorGlobal = nullptr;
void manejarSignal(int signal){
  if(servidorGlobal){
    cout << "Cerrando servidor mediante Ctrl+C"<< endl;
    servidorGlobal->stop();
  }
};

string escaparJson(string texto){
  string limpio = "";
  for(size_t i = 0; i < texto.length(); i++){
    char c = texto[i];
    if(c == '\"') limpio += "\\\"";
    else if(c == '\\') limpio += "\\\\";
    else if(c == '\n') limpio += "\\n";
    else if(c == '\r') continue;
    else limpio += c;
  };
  return limpio;
};

class Paciente{ //clase que guarda los datos del paciente
    private:
        string nombre;
        string tipoDocumento;
        string sexo;
        string documento;
        int edad;
        string eps;
        string correo;
        string telefono;
        string direccion;
    public:
        Paciente(){}
        Paciente(string _sexo, string _tipoDocumento){
            nombre = "NA";
            tipoDocumento = _tipoDocumento;
            sexo = _sexo;
            documento = "NA";
            edad = 0;
            correo = "NA";
            telefono = "NA";
            direccion = "NA";
        }
        Paciente(string _nombre, string _documento, string _tipoDocumento, int _edad, string _sexo, string _eps, string _correo, string _telefono, string _direccion) {
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
        string getSexo(){
            return sexo;
        }
        int getEdad(){
            return edad;
        }
        string getDocumento(){
            return documento;
        }
        string getNombre(){
            return nombre;
        }
        string getTipoDocumento(){
          return tipoDocumento;
        }
        string getEps(){
          return eps;
        }
        string getCorreo(){
          return correo;
        }
        string getTelefono(){
          return telefono;
        }
        string getDireccion(){
          return direccion;
        }
};
class AcudientePaciente{ //guarda los datos del acudiente si aplica
    private:
        string nombre;
        string tipoDocumento;
        string documento;
        string parentesco;
        string correo;
        string telefono;
        Paciente* paciente;
    public:
        AcudientePaciente(string _nombre, string _tipoDocumento, string _documento, string _parentesco, string _correo, string _telefono, Paciente* _paciente){
            nombre = _nombre;
            tipoDocumento = _tipoDocumento;
            documento = _documento;
            parentesco = _parentesco;
            correo = _correo;
            telefono = _telefono;
            paciente = _paciente;
        }
        string getNombre(){ return nombre; }
        string getTipoDocumento(){ return tipoDocumento; }
        string getDocumento(){ return documento; }
        string getParentesco(){ return parentesco; }
        string getCorreo(){ return correo; }
        string getTelefono(){ return telefono; }
};
class Triage{ //clase que viene siendo donde se guardan los datos de la consulta
    private:
        Paciente* paciente;
        AcudientePaciente* acudiente;
        bool embarazo;
        float temperatura;
        int estatura;
        float peso;
        int presionSistolica;
        int presionDiastolica;
        int frecuenciaCardiaca;
        int frecuenciaRespiratoria;
        int saturacion;
        int escala;
        int glasgow;
        string motivo;
        string alergias;
        string enfermedades;
        string cirugias;
        string observaciones;
        time_t horaLlegada;
        time_t tiempoEspera;
        int turnoLlegada;
        int triage;
        bool buscarPalabras(string texto, string palabras[], int tamano){ //busca palabras
            for(int i = 0; i<tamano;i++){
                if(texto.find(palabras[i])!=string::npos){
                    return true;
                }
            }
            return false;
        }
        string normalizar(string palabra){ //Normaliza los textos para evitar errores
            for(char &c : palabra){
                c = toupper(c);
            }
            struct Reemplazo { string conTilde; string sinTilde; };
            Reemplazo lista[] = {
                {"Á", "A"}, {"É", "E"}, {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"},
                {"á", "A"}, {"é", "E"}, {"í", "I"}, {"ó", "O"}, {"ú", "U"}, {"ü", "U"}, {"Ü", "U"}
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
        int calcularTriage(){ //calcula el triage dependiendo del caso del paciente
            tiempoEspera = obtenerMinutosEspera();
            string texto = motivo+" "+observaciones;
            
            string palabrasNivel1[] = {"DISNEA DE REPOSO", "HABLA ENTRECORTADA", "USO MARCADO DE MUSCULOS ACCESORIOS", "CIANOSIS DISTAL", 
                "CIANOSIS PERIORAL", "ALTERACION DEL ESTADO DE ALERTA", "TRAUMA SEVERO", "POLITRAUMATISMO CRITICO", "PERDIDA DE CONSCIENCIA PROFUNDA", 
                "COMA", "ESTUPOR", "ACV", "ACCIDENTE CEREBRO VASCULAR", "HEMORRAGIA SEVERA", "QUEMADURAS EXTENSAS", "RIESGO DE PERDER UN MIEMBRO", 
                "RIESGO DE PERDER UN ORGANO", "SHOCK", "PERDIDA DE CONCIENCIA PROFUNDA", "PARO RESPIRATORIO", "APNEA", "INSUFICIENCIA RESPIRATORIA AGUDA",
                "PARO CARDIORESPIRATORIO", "PCR", "DOLOR TORACICO OPRESIVO", "INFARTO", "IAM", "HEMORRAGIA EXANGUINANTE", "CONVULSION", "ESTADO EPILEPTICO"};
            int tamano1 = sizeof(palabrasNivel1)/sizeof(palabrasNivel1[0]);    
            string palabrasNivel2[] = {"DOLOR MUY INTENSO", "DOLOR EN EL PECHO", "COLICOS NEFRITICOS", "INTOXICACION AGUDA", "ENVENENAMIENTO", "HEMORRAGIAS",
            "FIEBRE ALTA", "DIFICULTAD RESPIRATORIA MODERADA", "TRAUMATISMO", "LESIONES GRAVES"};
            int tamano2 = sizeof(palabrasNivel2)/sizeof(palabrasNivel2[0]);
            string epoc[] = {"EPOC", "ENFERMEDAD PULMONAR CRONICA"};
            bool encontroEpoc = buscarPalabras(observaciones, epoc, 2);
            string debilidad[] = {"DEBILIDAD EXTREMA", "CONFUSION"};
            string palabrasNivel3[] = {"DOLOR MODERADO", "VOMITO PERSISTENTE", "DIARREA RECUENTE", "DESHIDRATACION MODERADA", "HERIDA PARA SUTURA", 
                "SUTURA", "REACCION ALERGICA LEVE", "ESGUINCE", "SOSPECHA DE FRACTURA", "TRAUMA MODERADO", "LUXACION", "RETENCION URINARIA", 
                "DOLOR ABDOMINAL MODERADO", "CRISIS ASMATICA LEVE", "HEMOPTISIS LEVE", "SANGRE EN LA ORINA", "HEMATURIA","REACCION ADVERSA A MEDICAMENTO", 
                "HISTERIA", "CRISIS DE ANSIEDAD"};
            int tamano3 = sizeof(palabrasNivel3)/sizeof(palabrasNivel3[0]);
            string factoresRiesgo[] = {"DIABETES", "HIPERTENSION", "INSUFICIENCIA CARDIACA", "CANCER", "INMUNOSUPRESION"};
            string palabrasNivel4[] = {"DOLOR LEVE", "MOLESTIA LEVE", "DOLOR DE VARIOS DIAS", "RESFRIADO COMUN", "RESFRIADO","DOLOR DE GARGANTA", "CUADRO GRIPAL", 
                "GRIPA", "INFECCION URINARIA LEVE", "MOLESTIA AL ORINAR", "DIARREA SIMPLE", "DIARREA SIN DESHIDRATACION", "MOLESTIA DIGESTIVA", "CONTUSION LEVE",
                "GOLPE LEVE", "ESGUINCE MENOR", "HERIDA SUPERFICIAL", "RASPON", "ABSCESO PEQUEÑO", "INFECCION LOCALIZADA"};
            int tamano4 = sizeof(palabrasNivel4) / sizeof(palabrasNivel4[0]);
            int triage = 5;
            if(frecuenciaCardiaca == 0 || buscarPalabras(texto, palabrasNivel1, tamano1) || saturacion < 80 || glasgow <= 8) triage = 1;
            else if(presionSistolica>=180 || presionDiastolica>110 || presionSistolica<=90 || frecuenciaCardiaca>=120||
                frecuenciaCardiaca<=50 || frecuenciaRespiratoria>24 || frecuenciaRespiratoria<10 || (saturacion < 90 && !encontroEpoc) 
                ||(saturacion<=85&&encontroEpoc) || temperatura >=39.5 || (temperatura<=35&&buscarPalabras(texto, debilidad, 2)) 
                || buscarPalabras(texto, palabrasNivel2, tamano2) || temperatura <= 34 || escala >= 7) triage = 2;
            else if(temperatura>=38 || presionSistolica>=140 || frecuenciaCardiaca>=100 || frecuenciaRespiratoria>=21
            ||buscarPalabras(texto, palabrasNivel3, tamano3) || escala >= 4;
            (paciente!= NULL && paciente->getEdad()>=65 && buscarPalabras(enfermedades, factoresRiesgo, 5))) triage = 3;
            else if(buscarPalabras(texto, palabrasNivel4, tamano4)) triage = 4;
            else triage = 5;

            if(triage==2 && tiempoEspera>30) triage = 1;
            else if(triage==3 && tiempoEspera>100) triage = 1;
            else if(triage==4 && tiempoEspera > 420) triage = 1;
            else if(triage==5 && tiempoEspera > 720) triage = 1;
            return triage;
        }
    public:
        Triage(){
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
            escala = 0; glasgow = 15;
            horaLlegada = time(0); 
            turnoLlegada = 0; 
            triage = 5;
        }
        Triage(Paciente* _paciente, bool _embarazo, float _temperatura, int _estatura, float _peso, int _presionSistolica, 
            int _presionDiastolica, int _frecuenciaCardiaca, int _frecuenciaRespiratoria, int _saturacion, int _escala, string _motivo, 
            string _alergias, string _enfermedades, string _cirugias, string _observaciones, int _glasgow, int _turno, AcudientePaciente* _acudiente = nullptr){
            paciente = _paciente;
            acudiente = _acudiente;
            if(paciente->getSexo()=="Masculino") embarazo = 0;
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
        AcudientePaciente* getAcudiente(){
          return acudiente;
          
        }
        int getTriage(){ //getters
            triage = calcularTriage();
            return triage;
        }
        int getHoraLlegada(){
            return horaLlegada;
        }
        int getTurnoLlegada(){
            return turnoLlegada;
        }
        Paciente* getPaciente(){
            return paciente;
        }
        float getTemperatura(){
          return temperatura;
        }
        bool getEmbarazo() { 
          return embarazo; 
        }
        int getEstatura() { 
          return estatura; 
        }
        float getPeso() { 
          return peso; 
        }
        int getPresionSistolica() { 
          return presionSistolica; 
        }
        int getPresionDiastolica() { 
          return presionDiastolica; 
        }
        int getFrecuenciaCardiaca() { 
          return frecuenciaCardiaca; 
        }
        int getFrecuenciaRespiratoria() { 
          return frecuenciaRespiratoria; 
        }
        int getSaturacion() { 
          return saturacion; 
        }
        int getEscala() { 
          return escala; 
        }
        int getGlasgow() { 
          return glasgow; 
        }
        string getMotivo() { 
          return motivo; 
        }
        string getAlergias() { 
          return alergias; 
        }
        string getEnfermedades() { 
          return enfermedades; 
        }
        string getCirugias() { 
          return cirugias; 
        }
        string getObservaciones() { 
          return observaciones; 
        }
        int obtenerMinutosEspera(){ //nos dice cuanto tiempo ha estado esperando el paciente
            auto ahora = chrono::system_clock::now();
            time_t horaActual = chrono::system_clock::to_time_t(ahora);
            double segundos = difftime(horaActual, horaLlegada);
            return (int)(segundos/60);
        }
};

class HistoriaClinica{
  private:
    Triage triageIngreso;
    string anamnesis;
    string diagnostico;
    string tratamiento;
    string indicaciones;
    string fechaHoraAtencion;
  public:
    HistoriaClinica(){}
    HistoriaClinica(Triage _triage, string _anamnesis, string _diagnostico, string _tratamiento, string _indicaciones, string _fechaHora){
      triageIngreso = _triage;
      anamnesis = _anamnesis;
      diagnostico = _diagnostico;
      tratamiento = _tratamiento;
      indicaciones = _indicaciones;
      fechaHoraAtencion = _fechaHora;
    };
    Triage getTriageIngreso(){ return triageIngreso; };
    string getAnamnesis(){ return anamnesis; };
    string getDiagnostico(){ return diagnostico; };
    string getTratamiento(){ return tratamiento; };
    string getIndicaciones(){ return indicaciones; };
    string getFechaHoraAtencion(){ return fechaHoraAtencion; };
};

struct NodoHistoria{
  public:
    HistoriaClinica dato;
    NodoHistoria * siguiente;
    NodoHistoria(HistoriaClinica h){
      dato = h;
      siguiente = NULL;
    };
};

class ListaHistorias{
  private:
    NodoHistoria * cabeza;
    NodoHistoria * ultimo;
  public:
    ListaHistorias(){
      cabeza = NULL;
      ultimo = NULL;
    };
    ~ListaHistorias(){
      NodoHistoria * actual = cabeza;
      while(actual != NULL){
        NodoHistoria * sig = actual->siguiente;
        delete actual;
        actual = sig;
      };
    };
    void insertarAlFinal(HistoriaClinica h){
      NodoHistoria * nuevo = new NodoHistoria(h);
      if(cabeza == NULL){
        cabeza = nuevo;
        ultimo = cabeza;
      }else{
        ultimo->siguiente = nuevo;
        ultimo = nuevo;
      };
    };
    NodoHistoria * getCabeza(){
      return cabeza;
    };
};

bool esMenor(Triage& a, Triage& b) { //Se usan por referencia en la memoria para que no gaste mas memoria
  if (a.getTriage() != b.getTriage()) return a.getTriage() < b.getTriage(); 
  if (a.getHoraLlegada() != b.getHoraLlegada()) return a.getHoraLlegada() < b.getHoraLlegada();
  return a.getTurnoLlegada() < b.getTurnoLlegada();
};
struct Nodo{
  public:
    Triage dato;
    Nodo * siguiente;
    Nodo(Triage t){
      dato = t;
      siguiente = NULL;
    }
};
struct Lista{
  private:
    Nodo * cabeza;
  public:
    Lista(){
      cabeza = NULL;
    }
    ~Lista() {
      Nodo* actual = cabeza;
      while (actual != nullptr) {
        Nodo* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
      }
    }
    void insertarOrdenado(Triage t){
      Nodo * nuevo = new Nodo(t);
      if(cabeza == NULL || esMenor(t, cabeza->dato)){
        nuevo->siguiente = cabeza;
        cabeza = nuevo;
        return;
      }
      Nodo * actual = cabeza;
      while(actual->siguiente != NULL && !esMenor(t, actual->siguiente->dato)){
        actual = actual->siguiente;
      }
      nuevo->siguiente = actual->siguiente;
      actual->siguiente = nuevo;
    }
    Nodo * getCabeza(){
      return cabeza;
    }
};
struct PilaAtendidos{
  private:
  Nodo * cabeza;
  int cantidad;
  public:
    PilaAtendidos(){
      cabeza = NULL;
      cantidad = 0;
    }
    void agregarPacienteAtendido(Triage t){
      Nodo * nuevo = new Nodo(t);
      if(cabeza==NULL) {
        cabeza = nuevo;
        nuevo->siguiente = NULL;
        cantidad = 1;
        return;
      }
      nuevo->siguiente = cabeza;
      cabeza = nuevo;
      cantidad++;
    };
    Triage deshacerAccion(){
      Nodo * temp = cabeza;
      if(cabeza==NULL) throw -1;
      if(cantidad==1){
        temp = cabeza;
        Triage paciente = temp->dato;
        delete[] temp;
        cabeza = NULL;
        cantidad = 0;
        return paciente;
      }
      temp = cabeza;
      Triage paciente = temp->dato;
      cabeza = cabeza->siguiente;
      delete[] temp;
      return paciente;
    }  
    Nodo * getTope(){
      return cabeza;
    }
};
struct ColaPrioridad{
  public:
    ColaPrioridad( int m = 8 ){    
      N = 0;
      tamano = m;
      datos = new Triage[tamano];
    };

    ~ColaPrioridad(){
      delete[] datos;
    };

    bool esvacia(){
      return N == 0;
    };

    int longitud(){
      return N;
    };

    Triage obtener( int i ){
      if(0 <= i && i < N){
        return datos[i];
      }else{
        throw -1;
      };
    };

    void agregar( Triage d ){
      grow(); 
      int i = N - 1;
      while (i >= 0 && esMenor(d, datos[i])) {
        datos[i + 1] = datos[i];
        i--;
      }
      datos[i + 1] = d;
      N++;
    };

    Triage eliminar_primero(){
      if (esvacia()) throw 0;
      Triage primero = datos[0];
      for (int i = 1; i < N; i++) {
        datos[i - 1] = datos[i];
      }
      N--; 
      return primero;
    };

  private:
    Triage * datos;
    int N;
    int tamano;
    double aureo = 1.618;

    void grow(){ //Se encarga de hacer crecer el arreglo dinamico
      if(N == tamano){
        int ntamano = (int)(tamano * aureo);
        if(ntamano <= tamano){ ntamano = tamano + 1; };
        Triage * ndatos = new Triage[ntamano];
        for( int i=0; i<tamano; i++ ){
          ndatos[i] = datos[i];
        };
        delete[] datos;
        datos = ndatos;
        tamano = ntamano;
      };
    };
};
int contadorTurno = 1;
ColaPrioridad colaTriage;
PilaAtendidos pilaAlta;
int main(){
    httplib::Server svr; //crea el servidor
    servidorGlobal = &svr;
    std::signal(SIGINT, manejarSignal);
    svr.Post("/api/triage", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        //Aqui es donde se reciben los datos de la interfaz
        Paciente* paciente = NULL;
        string tipoDocumentoPaciente = req.tieneParametro("tipoDocumentoPaciente") ? req.getParametro("tipoDocumentoPaciente") : "SI";
        string sexo = req.tieneParametro("sexo") ? req.getParametro("sexo") : "NA";
        string documentoPaciente = "NA";
        if(tipoDocumentoPaciente=="AS"||tipoDocumentoPaciente=="MS"||tipoDocumentoPaciente=="SI") paciente = new Paciente(sexo, tipoDocumentoPaciente);
        else{
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
        if(parentesco=="nadie"||parentesco == "NA") {}
        else{
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
        int presionDiastolica =(req.tieneParametro("presionDiastolica") && !req.getParametro("presionDiastolica").empty()) ? stoi(req.getParametro("presionDiastolica")) : 90;
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
        Triage triage(paciente, embarazo, temperatura, estatura, peso, presionSistolica, presionDiastolica, frecuenciaCardiaca, 
            frecuenciaRespiratoria, saturacion, escalaDolor, motivo, alergias, enfermedades, cirugias, observaciones, escalaGlasgow, contadorTurno++);
        colaTriage.agregar(triage);
        int nivelCalculado = triage.getTriage();
        cout << "Triage calculado exitosamente: NIVEL " << nivelCalculado << endl;
        cout << "======================================" << endl;
        string jsonRespuesta = "{\"status\":\"ok\", \"triage\":" +to_string(nivelCalculado) + "}";
        res.set_content(jsonRespuesta, "application/json");
    });
    svr.Get("/api/pacientes", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        Lista listaOrdenada;
        for (int i = 0; i<colaTriage.longitud();i++){ //la copia del arreglo dinamico en una lista
          listaOrdenada.insertarOrdenado(colaTriage.obtener(i));
        }
      string json = "[";
      Nodo * actual = listaOrdenada.getCabeza();
      bool primero = true;
      while(actual != NULL) {
        Triage t = actual->dato;
        if(!primero) json += ",";
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
    svr.Get("/api/datosPaciente", [&svr](const httplib::Request& req, httplib::Response& res) { //enviar los daos para la consulta
      res.set_header("Access-Control-Allow-Origin", "*");
      if(!colaTriage.esvacia()){
        Triage cabeza = colaTriage.obtener(0);
        Paciente * paciente = cabeza.getPaciente();
        string json = "{"; 
          json += "\"nombre\": \""+paciente->getNombre()+"\",";
          json += "\"tipoDocumento\": \""+paciente->getTipoDocumento()+"\",";
          json += "\"documento\": \""+paciente->getDocumento()+"\",";
          json += "\"sexo\": \""+paciente->getSexo()+"\",";
          json += "\"edad\": \""+to_string(paciente->getEdad())+"\",";
          json += "\"eps\": \""+paciente->getEps()+"\","; 
          json += "\"correo\": \""+paciente->getCorreo()+"\",";
          json += "\"telefono\": \""+paciente->getTelefono()+"\",";
          json += "\"embarazo\": ";
          if(cabeza.getEmbarazo()) json += "Si\",";
          else json += "\"No\",";
          json += "\"temperatura\": \""+to_string(cabeza.getTemperatura())+"\",";
          json += "\"estatura\": \""+to_string(cabeza.getEstatura())+"\",";
          json += "\"peso\": \""+to_string(cabeza.getPeso())+"\",";
          json += "\"frecuenciaCardiaca\": \""+to_string(cabeza.getFrecuenciaCardiaca())+"\",";
          json += "\"presionSistolica\": \""+to_string(cabeza.getPresionSistolica())+"\",";
          json += "\"presionDiastolica\": \""+to_string(cabeza.getPresionDiastolica())+"\",";
          json += "\"frecuenciaRespiratoria\": \""+to_string(cabeza.getFrecuenciaRespiratoria())+"\",";
          json += "\"saturacion\": \""+to_string(cabeza.getSaturacion())+"\",";
          json += "\"glasgow\": \""+to_string(cabeza.getGlasgow())+"\",";
          json += "\"escala\": \""+to_string(cabeza.getEscala())+"\",";
          json += "\"motivo\": \""+cabeza.getMotivo()+"\",";
          json += "\"alergias\": \""+cabeza.getAlergias()+"\",";
          json += "\"enfermedades\": \""+cabeza.getEnfermedades()+"\",";
          json += "\"cirugias\": \""+cabeza.getCirugias()+"\",";
          json += "\"observaciones\": \""+cabeza.getObservaciones()+"\"";
          json += "}";
        res.set_content(json, "application/json");
      }else res.set_content("{\"hayPaciente\": false, \"nombre\": \"No hay pacientes en espera\"}", "application/json");
    });
    svr.Post("/api/atenderPaciente", [](const httplib::Request& req, httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "*");
      if(!colaTriage.esvacia()){
        Triage paciente = colaTriage.eliminar_primero();
        pilaAlta.agregarPacienteAtendido(paciente);
        res.set_content("{\"status\": \"ok\"}", "application/json");
      }else res.set_content("{\"status\": \"vacio\"}", "application/json");
    });
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
    svr.Get("/api/historialPacientes", [](const httplib::Request& req, httplib::Response& res){
      res.set_header("Access-Control-Allow-Origin", "*");
      if (!colaTriage.esvacia()) {
        Triage cabeza = colaTriage.obtener(0);
        Paciente * paciente = cabeza.getPaciente();
        
        string nom = (paciente != nullptr) ? paciente->getNombre() : "Sin nombre";
        string doc = (paciente != nullptr) ? paciente->getDocumento() : "NA";

        string json = "{"; 
        json += "\"hayPaciente\": true,";
        json += "\"nombre\": \"" + escaparJson(nom) + "\",";
        json += "\"documento\": \"" + escaparJson(doc) + "\",";
        json += "\"triage\": " + to_string(cabeza.getTriage()) + ",";
        json += "\"motivo\": \"" + escaparJson(cabeza.getMotivo()) + "";
        json += "}";
        res.set_content(json, "application/json");
      } else {
        res.set_content("{\"hayPaciente\": false, \"nombre\": \"No hay pacientes en espera\"}", "application/json");
      }
    }); 
    svr.Get("/api/stop", [&svr](const httplib::Request& req, httplib::Response& res) { //cierra el servidor
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("{\"status\":\"servidor deteniendose\"}", "application/json");
        svr.stop();
    });
    svr.listen("0.0.0.0", 8080); //le coloca una ip (local) y un puerto y lo manda por el chat de la consola
    return 0;
};
