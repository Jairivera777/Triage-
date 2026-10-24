#pragma once

#include "httplib.h"
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>

using namespace std;

void manejarSignal(int signal);
extern httplib::Server* servidorGlobal;
// Clase que guarda los datos básicos del paciente
class Paciente {
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
    Paciente();
    Paciente(string _sexo, string _tipoDocumento);
    Paciente(string _nombre, string _documento, string _tipoDocumento, int _edad,
             string _sexo, string _eps, string _correo, string _telefono,
             string _direccion);

    string getSexo();
    int getEdad();
    string getDocumento();
    string getNombre();
    string getTipoDocumento();
    string getEps();
    string getCorreo();
    string getTelefono();
    string getDireccion();
};

// Guarda los datos del acudiente si aplica
class AcudientePaciente {
private:
    string nombre;
    string tipoDocumento;
    string documento;
    string parentesco;
    string correo;
    string telefono;
    Paciente* paciente;

public:
    AcudientePaciente(string _nombre, string _tipoDocumento, string _documento,
                      string _parentesco, string _correo, string _telefono,
                      Paciente* _paciente);

    string getNombre();
    string getTipoDocumento();
    string getDocumento();
    string getParentesco();
    string getCorreo();
    string getTelefono();
};

// Clase central del sistema: guarda los datos de la consulta y calcula el triage
class Triage {
private:
    Paciente* paciente;
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
    int triageActual;
    int triageOriginal;
    bool isPrimero;
    bool buscarPalabras(string texto, string palabras[], int tamano);
    string normalizar(string palabra);
    int calcularTriage();

public:
    Triage();
    Triage(Paciente* _paciente, bool _embarazo, float _temperatura, int _estatura,
           float _peso, int _presionSistolica, int _presionDiastolica,
           int _frecuenciaCardiaca, int _frecuenciaRespiratoria, int _saturacion,
           int _escala, string _motivo, string _alergias, string _enfermedades,
           string _cirugias, string _observaciones, int _glasgow, int _turno);
    int getTriage();
    int getHoraLlegada();
    int getTurnoLlegada();
    Paciente* getPaciente();
    float getTemperatura();
    bool getEmbarazo();
    int getEstatura();
    float getPeso();
    int getPresionSistolica();
    int getPresionDiastolica();
    int getFrecuenciaCardiaca();
    int getFrecuenciaRespiratoria();
    int getSaturacion();
    int getEscala();
    int getGlasgow();
    string getMotivo();
    string getAlergias();
    string getEnfermedades();
    string getCirugias();
    string getObservaciones();
    int getMinutosEspera();
};

class HistoriaClinica {
private:
    Triage triageIngreso;
    string anamnesis;
    string diagnostico;
    string tratamiento;
    string indicaciones;
    string fechaHoraAtencion;

public:
    HistoriaClinica();
    HistoriaClinica(Triage _triage, string _anamnesis, string _diagnostico,
                   string _tratamiento, string _indicaciones, string _fechaHora);
    Triage getTriageIngreso();
    string getAnamnesis();
    string getDiagnostico();
    string getTratamiento();
    string getIndicaciones();
    string getFechaHoraAtencion();
};

struct NodoHistoria {
    HistoriaClinica dato;
    NodoHistoria* siguiente;
    NodoHistoria(HistoriaClinica h);
};

class ListaHistorias {
private:
    NodoHistoria* cabeza;
    NodoHistoria* ultimo;

public:
    ListaHistorias();
    ~ListaHistorias();
    void insertarAlFinal(HistoriaClinica h);
    NodoHistoria* getCabeza();
};

bool esMenor(Triage& a, Triage& b);

// Nodo para listas y colas
struct Nodo {
    Triage dato;
    Nodo* siguiente;
    Nodo(Triage t);
};

// Pila para guardar pacientes atendidos y permitir deshacer
struct PilaAtendidos {
private:
    Nodo* cabeza;
    int cantidad;

public:
    PilaAtendidos();
    void agregarPacienteAtendido(Triage t);
    Triage deshacerAccion();
    Nodo* getCabeza();
};

// Cola simple para pacientes de una misma prioridad
struct Cola {
private:
    Nodo* cabeza;
    Nodo* ultimo;

public:
    Cola();
    bool isVacia();
    void insertarUltimo(Triage t);
    void insertarPrimero(Triage t);
    Triage atenderPaciente();
    Nodo* getCabeza();
};

// Lista ordenada para mostrar pacientes en orden por triage
struct Lista {
private:
    Nodo* cabeza;
    Nodo* ultimo;

public:
    Lista();
    void insertarOrdenado(Triage t);
    Nodo* getCabeza();
};

// Cola con prioridad por niveles de triage
struct ColaPrioridad {
private:
    Cola colas[5];

public:
    ColaPrioridad();
    void agregar(Triage t);
    void deshacer(Triage t);
    Triage atender();
    bool isVacia();
    int longitud();
    Cola getCola(int i);
    Nodo * getPrimero();
    Nodo * getSegundo();
};

extern int contadorTurno;
extern ColaPrioridad colaTriage;
extern PilaAtendidos pilaAlta;

void inicializarServidor(httplib::Server& svr);
